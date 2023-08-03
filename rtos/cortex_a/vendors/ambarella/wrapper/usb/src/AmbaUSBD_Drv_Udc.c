/**
 *  @file AmbaUSBD_Drv_Udc.c
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
 *  @details USB UDC driver
 */

#include <AmbaTypes.h>
#define AMBA_KAL_SOURCE_CODE
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_Buffer.h>
#include <usbx/ux_device_stack.h>

#define USBD_FLAG_USBD_RTSL_MUTEX_INIT     0x00000001UL
#define USBD_FLAG_USBD_RX_MUTEX_INIT       0x00000002UL
#define USBD_FLAG_USBD_RX_STATE_MUTEX_INIT 0x00000004UL
#define USBD_FLAG_USBD_DRIVER_INIT         0x00000008UL
#define USBD_FLAG_USBD_SETUP_EVENT_INIT    0x00000010UL
#define USBD_FLAG_USBD_INT_QUEUE_INIT      0x00000020UL

#define USBD_EP_ISO_MAX_BUFFER_PER_XFER    (0x02UL << 13U)           // [13:0] for Tx Bytes

static USBD_SYS_CONFIG_s *usbd_sys_config_udc_local  = NULL;
static UINT32             flag_udc_init              = 0;
static USBD_UDC_s         local_usbd_udc __attribute__((section(".bss.noinit")));
static UDC_CONTROL_EP_DESCS_s ctrl_ep_desc __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")));
static UINT32 flag_txfifo_double_buffer = 1;

// RTSL mutex shall be created once and won't be destroyed.
// Thus we don't put it in udc instance.
static AMBA_KAL_MUTEX_t    mutex_rtsl;
static UDC_ISO_REQUEST_s   iso_req_on_going;
static UINT32              flag_iso_tx_ongoing = 0;

void USBD_UdcFlagIsoTxOngoingSet(UINT32 Flag)
{
    flag_iso_tx_ongoing = Flag;
}

static UINT32 udc_get_endpoint_type(const UX_SLAVE_ENDPOINT *UsbxEndpoint)
{
    return (UsbxEndpoint->ux_slave_endpoint_descriptor.bmAttributes & (ULONG)UX_MASK_ENDPOINT_TYPE) |
           (UsbxEndpoint->ux_slave_endpoint_descriptor.bEndpointAddress & (ULONG)UX_ENDPOINT_DIRECTION);
}

static UINT32 udc_is_control_endpoint_type(UINT32 UsbxEndpointType)
{
    UINT32 uret;

    if ((UsbxEndpointType & 0x03U) == (UINT32)UX_CONTROL_ENDPOINT) {
        uret = 1;
    } else {
        uret = 0;
    }
    return uret;
}

static UINT32 udc_get_endpoint_max_packet_size(const UX_SLAVE_ENDPOINT *UsbxEndpoint)
{
    return UsbxEndpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
}

static UINT32 buffer_cache_alignment_check(const UINT8 *Buffer)
{
    UINT32 address = USB_UtilityU8PtrToU32Addr(Buffer);
    UINT32 mask = AMBA_CACHE_LINE_SIZE - 1U;
    UINT32 uret;

    if ((address & mask) != 0U) {
        uret = USB_ERR_FAIL;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "transfer buffer 0x%x is not %d-aligned", address, AMBA_CACHE_LINE_SIZE, 0, 0, 0);
    } else {
        uret = USB_ERR_SUCCESS;
    }

    return uret;
}

static void usb_error_notify(UINT32 ErrCode, UINT32 UserSpecific)
{
    if ((usbd_sys_config_udc_local != NULL) &&
        (usbd_sys_config_udc_local->SystemEventCallback != NULL)) {
        if (usbd_sys_config_udc_local->SystemEventCallback(ErrCode, UserSpecific, 0, 0) != 0U) {
            // ignore error
        }
    }
}

static void control_endpoint_create(void)
{
    UX_SLAVE_DEVICE     *usbx_device   = &_ux_system_slave->ux_system_slave_device;
    UDC_ENDPOINT_INFO_s *udc_ep_info   = &local_usbd_udc.DcdEndpointsInfoArray[0];
    UX_SLAVE_ENDPOINT   *usbx_endpoint = &usbx_device->ux_slave_device_control_endpoint;

    // Get the pointer to the device.
    usbx_device->ux_slave_device_control_endpoint.ux_slave_endpoint_descriptor.bEndpointAddress = 0;
    usbx_device->ux_slave_device_control_endpoint.ux_slave_endpoint_descriptor.bmAttributes     = 0;
    usbx_device->ux_slave_device_control_endpoint.ux_slave_endpoint_descriptor.wMaxPacketSize   = 64;
    // Hook control endpoint.
    udc_ep_info->UxEndpoint             = usbx_endpoint;
    usbx_endpoint->ux_slave_endpoint_ed = USB_UtilityUdcEd2VoidP(udc_ep_info);
    USBD_UdcEndpointBufferAllocate(&usbx_device->ux_slave_device_control_endpoint);
}

static void control_endpoint_destroy(void)
{
    UX_SLAVE_DEVICE     *usbx_device   = &_ux_system_slave->ux_system_slave_device;
    UDC_ENDPOINT_INFO_s *udc_ep_info   = &local_usbd_udc.DcdEndpointsInfoArray[0];
    UX_SLAVE_ENDPOINT   *usbx_endpoint = &usbx_device->ux_slave_device_control_endpoint;

    // Get the pointer to the device.
    udc_ep_info->UxEndpoint             = NULL;
    usbx_endpoint->ux_slave_endpoint_ed = NULL;
    USBD_UdcEndpointBufferDestroy(&usbx_device->ux_slave_device_control_endpoint);
}

static UINT32 low_level_mutex_get(void)
{
    UINT32 uret;

    if (mutex_rtsl.tx_mutex_id == 0U) {
        // If it's not inited, we just bypass the lock
        uret = USB_ERR_SUCCESS;
    } else {
        if (USB_UtilityMutexTake(&mutex_rtsl, USB_WAIT_FOREVER) != USB_ERR_SUCCESS) {
            uret = USB_ERR_MUTEX_LOCK_FAIL;
        } else {
            uret = USB_ERR_SUCCESS;
        }
    }

    return uret;
}

static UINT32 low_level_mutex_put(void)
{
    UINT32 uret;

    if (mutex_rtsl.tx_mutex_id == 0U) {
        // If it's not inited, we just bypass the unlock
        uret = USB_ERR_SUCCESS;
    } else {
        if (USB_UtilityMutexGive(&mutex_rtsl) != USB_ERR_SUCCESS) {
            uret = USB_ERR_MUTEX_UNLOCK_FAIL;
        } else {
            uret = USB_ERR_SUCCESS;
        }
    }

    return uret;
}

static UINT32 rtsl_mutex_init(void)
{
    UINT32 uret;
    static RTSL_USB_Mutex_s UdcRtslMutexCb = {
        low_level_mutex_get,
        low_level_mutex_put
    };

    if (USB_UtilityMutexCreate(&mutex_rtsl) != USB_ERR_SUCCESS) {
        // shall not be here.
        uret = USB_ERR_RTSL_MUTEX_CREATE_FAIL;
    } else {
        uret = AmbaRTSL_USBRegisterMutex(&UdcRtslMutexCb);
        if (uret != 0U) {
            // shall not be here.
            uret = USB_ERR_SUCCESS;
        } else {
            uret = USB_ERR_SUCCESS;
        }
    }

    return uret;
}

static UINT32 resource_rxmutex_create(void)
{
    UINT32 uret = USB_ERR_SUCCESS;
    if ((flag_udc_init & USBD_FLAG_USBD_RX_MUTEX_INIT) == 0U) {
        uret = USB_UtilityMutexCreate(&local_usbd_udc.RxMutex);
        if (uret == USB_ERR_SUCCESS) {
            flag_udc_init |= USBD_FLAG_USBD_RX_MUTEX_INIT;
        } else {
            uret = USB_ERR_RX_MUTEX_CREATE_FAIL;
        }
    }
    return uret;
}
static UINT32 resource_rx_state_mutex_create(void)
{
    UINT32 uret = USB_ERR_SUCCESS;
    if ((flag_udc_init & USBD_FLAG_USBD_RX_STATE_MUTEX_INIT) == 0U) {
        uret = USB_UtilityMutexCreate(&local_usbd_udc.RxStateMutex);
        if (uret == USB_ERR_SUCCESS) {
            flag_udc_init |= USBD_FLAG_USBD_RX_STATE_MUTEX_INIT;
        } else {
            uret = USB_ERR_RX_STATE_MUTEX_CREATE_FAIL;
        }
    }
    return uret;
}

static UINT32 resource_ctrl_eventflag_create(void)
{
    UINT32 uret = USB_ERR_SUCCESS;
    if ((flag_udc_init & USBD_FLAG_USBD_SETUP_EVENT_INIT) == 0U) {
        if (USB_UtilityEventCreate(&local_usbd_udc.CtrlReqEventFlag) == 0U) {
            uret           = USB_ERR_SUCCESS;
            flag_udc_init |= USBD_FLAG_USBD_SETUP_EVENT_INIT;
        } else {
            uret = USB_ERR_CTRL_EVENTFLAG_CREATE_FAIL;
        }
    }
    return uret;
}

static UINT32 resource_isr_queue_create(void)
{
    static UINT8 isr_queue_memory[USBD_ISR_QUEUE_MEM_SIZE] __attribute__((section(".bss.noinit")));
    UINT32 uret = USB_ERR_SUCCESS;
    if ((flag_udc_init & USBD_FLAG_USBD_INT_QUEUE_INIT) == 0U) {
        USB_UtilityMemorySet(isr_queue_memory, 0, sizeof(isr_queue_memory));
        uret = USB_UtilityQueueCreate(&local_usbd_udc.IsrRequestQueue,
                                      isr_queue_memory,
                                      sizeof(UDC_ISR_REQUEST_s),
                                      MAX_ISR_OPERATIONS);
        if (uret == USB_ERR_SUCCESS) {
            flag_udc_init |= USBD_FLAG_USBD_INT_QUEUE_INIT;
        } else {
            // Shall not be here.
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "resource_create(): ISR Queue Create Fail");
            uret = USB_ERR_ISR_QUEUE_CREATE_FAIL;
        }
    }
    return uret;
}

static UINT32 resource_create(void)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if ((flag_udc_init & USBD_FLAG_USBD_RTSL_MUTEX_INIT) == 0U) {
        uret = rtsl_mutex_init();
        if (uret == USB_ERR_SUCCESS) {
            flag_udc_init |= USBD_FLAG_USBD_RTSL_MUTEX_INIT;
        }
    }

    if (uret == USB_ERR_SUCCESS) {
        uret = resource_rxmutex_create();
    }

    if (uret == USB_ERR_SUCCESS) {
        uret = resource_rx_state_mutex_create();
    }

    // Create EventFlag for Control Request task here. No delete.
    if (uret == USB_ERR_SUCCESS) {
        uret = resource_ctrl_eventflag_create();
    }

    // Create ISR queue.
    if (uret == USB_ERR_SUCCESS) {
        uret = resource_isr_queue_create();
    }

    return uret;
}

static void parameters_init(void)
{
    static UINT32 init = 0;

    if (init == 0U) {
        // Initialize the connect change instance.
        USB_UtilityMemorySet(&local_usbd_udc, 0, sizeof(USBD_UDC_s));
        init = 1;
    } else {
        // do not reset all. Resources should not be deleted.
        local_usbd_udc.UxDcdOwner = NULL;
        USB_UtilityMemorySet(local_usbd_udc.DcdEndpointsInfoArray, 0, sizeof(UDC_ENDPOINT_INFO_s) * UDC_MAX_ENDPOINT_NUM);
        local_usbd_udc.RxFifoMax  = 0;
        local_usbd_udc.TxFifoUsed = 0;
        local_usbd_udc.TxFifoMax  = 0;
        USB_UtilityMemorySet(&local_usbd_udc.EndpointMapInfo, 0, sizeof(UDC_ENDPOINT_MAP_INFO_s));
        local_usbd_udc.CurrentFrameNumber  = 0;
        local_usbd_udc.DisableDoubleBuffer = 0;
    }

    // Initialize the control descriptor.
    USB_UtilityMemorySet(&ctrl_ep_desc, 0, sizeof(UDC_CONTROL_EP_DESCS_s));

    flag_txfifo_double_buffer = 1;
}

static void phy_setup(void)
{
    // pollo - 2014/08/06 - According to Mahendra, setting this bit to 1 will save more power.
    AmbaRTSL_RctSetUsb0Commononn(1);
    // Setup Phy0 Onwer.
    USB_PhyPhy0OwnerSet(UDC_OWN_PORT);
    // Call USB_PhySignalQualityAdjust() here!
    // Power on PLL.
    USB_PhyPllEnable(1, UPORT_MODE_DEVICE);
}

static void driver_init(void)
{
    UX_SLAVE_DCD *ux_dcd;
    USBD_UDC_s *udc = &local_usbd_udc;
    void *des_ptr;

    if ((flag_udc_init & USBD_FLAG_USBD_DRIVER_INIT) == 0U) {
        // Get the pointer to the USBX DCD.
        ux_dcd = &_ux_system_slave->ux_system_slave_dcd;

        // The controller initialized here is of UDC2.
        ux_dcd->ux_slave_dcd_controller_type = USBD_SLAVE_CONTROLLER_TYPE_UDC2;

        // Set the pointer to the DCD.
        USB_UtilityMemoryCopy(&des_ptr, &udc, sizeof(void*));
        ux_dcd->ux_slave_dcd_controller_hardware = des_ptr;

        // Save the base address of the controller.
        ux_dcd->ux_slave_dcd_io = AmbaRTSL_USBGetBaseAddress();

        // Set the generic DCD owner.
        udc->UxDcdOwner = ux_dcd;

        // Initialize the function collector for this DCD.
        ux_dcd->ux_slave_dcd_function = USBD_DrvDispatchFunction;

        // Set the state of the controller to OPERATIONAL now.
        ux_dcd->ux_slave_dcd_status = UX_DCD_STATUS_OPERATIONAL;

        udc->RxFifoMax  = (256 * 4); // 256*4 RX FIFO
        udc->TxFifoMax  = (576 * 4); // 576*4 TX FIFO
        udc->TxFifoUsed = 0;

        flag_udc_init |= USBD_FLAG_USBD_DRIVER_INIT;
    }
}

static void driver_deinit(void)
{
    UX_SLAVE_DCD *ux_dcd;

    if ((flag_udc_init & USBD_FLAG_USBD_DRIVER_INIT) != 0U) {
        // Get the pointer to the USBX DCD.
        ux_dcd = &_ux_system_slave->ux_system_slave_dcd;

        // Unhook the UDC from USBX
        ux_dcd->ux_slave_dcd_controller_type     = 0;
        ux_dcd->ux_slave_dcd_controller_hardware = NULL;
        ux_dcd->ux_slave_dcd_io                  = 0;
        ux_dcd->ux_slave_dcd_function            = NULL;
        ux_dcd->ux_slave_dcd_status              = UX_DCD_STATUS_HALTED;

        flag_udc_init &= ~USBD_FLAG_USBD_DRIVER_INIT;
    }
}

static void rx_mutex_get(UINT32 Timeout)
{
    if ((flag_udc_init & USBD_FLAG_USBD_RX_MUTEX_INIT) != 0U) {
        if (USB_UtilityMutexTake(&local_usbd_udc.RxMutex, Timeout) != USB_ERR_SUCCESS) {
            // shall not be here.
        }
    }
}

static void rx_mutex_put(void)
{
    if ((flag_udc_init & USBD_FLAG_USBD_RX_MUTEX_INIT) != 0U) {
        if (USB_UtilityMutexGive(&local_usbd_udc.RxMutex) != USB_ERR_SUCCESS) {
            // shall not be here.
        }
    }
}

static UINT32 rx_swfifo_allocate(const USBD_UDC_s *Udc, UDC_ENDPOINT_INFO_s *UdcEpInfo, UINT32 LogAddr)
{
    UINT32 uret                            = USB_ERR_SUCCESS;
    UDC_RDMA_SUB_DESC_INFO_s *sub_dma_info = &UdcEpInfo->SubDmaInfo;

    (void)LogAddr;

    // create DMA descriptor if NULL
    if (UdcEpInfo->RxSwFifoBase == NULL) {
        if (Udc->RxFifoMax <= UBUF_SIZE_UDC_SW_FIFO) {
            UdcEpInfo->RxSwFifoSize = Udc->RxFifoMax;

            uret = USB_BufferU8Attach(UBUF_UDC_SW_FIFO, &UdcEpInfo->RxSwFifoBase);

            if (uret == USB_ERR_SUCCESS) {
                udc_rxfifo_init_list_head(&UdcEpInfo->RxSwFifoList, NULL);
            } else {
                UdcEpInfo->RxSwFifoSize = 0;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "rx_swfifo_allocate: can't allocate RxFifo for EP 0x%X.", LogAddr, uret, 0, 0, 0);
                uret = UX_MEMORY_INSUFFICIENT;
            }
        } else {
            uret = UX_MEMORY_INSUFFICIENT;
        }
    }

    if ((uret == USB_ERR_SUCCESS) && (sub_dma_info->Buffer == NULL)) {
        uret = USB_BufferU8Attach(UBUF_UDC_ALT, &sub_dma_info->Buffer);
        if (uret == USB_ERR_SUCCESS) {
            USB_UtilityMemorySet(sub_dma_info->Buffer, 0, 512);
            USB_UtilityCacheFlushUInt8(sub_dma_info->Buffer, 512);
        } else {
            uret = UX_MEMORY_INSUFFICIENT;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "rx_swfifo_allocate: can't allocate Sub Dma buffer for EP 0x%X.", LogAddr, uret, 0, 0, 0);
        }
    }

    if (uret == USB_ERR_SUCCESS) {
        sub_dma_info->DmaDesc.Status      = USBD_DMA_LAST_DESC;
        sub_dma_info->DmaDesc.Reserved    = 0xFFFFFFFFU;
        sub_dma_info->DmaDesc.DataPtr     = 0;
        sub_dma_info->DmaDesc.NextDescPtr = 0;
        USB_UtilityCacheFlushDataDesc(&sub_dma_info->DmaDesc, sizeof(UDC_DATA_DESC_s));
    }

    return uret;
}

static void rx_fifo_destroy(UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    UDC_RDMA_SUB_DESC_INFO_s *sub_dma_info = &UdcEpInfo->SubDmaInfo;

    if (UdcEpInfo->Direction == USBD_EP_DIR_OUT) {
        // endpoint Out
        if (UdcEpInfo->RxSwFifoBase == NULL) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "rx_fifo_destroy(): [0x%x] RxFifo is NULL",
                                  UdcEpInfo->UxEndpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                  0, 0, 0, 0);
        } else {
            USB_BufferU8Detach(UBUF_UDC_SW_FIFO, UdcEpInfo->RxSwFifoBase);
            UdcEpInfo->RxSwFifoBase = NULL;
        }

        if (sub_dma_info->Buffer == NULL) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "rx_fifo_destroy: [0x%x] Sub Dma buffer is NULL",
                                  UdcEpInfo->UxEndpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                  0, 0, 0, 0);
        } else {
            USB_BufferU8Detach(UBUF_UDC_ALT, sub_dma_info->Buffer);
            sub_dma_info->Buffer = NULL;
        }
    }
}

static void controller_endpoint_setup(
    USBD_UDC_s *               Udc,
    const UX_SLAVE_ENDPOINT   *Endpoint,
    const UDC_ENDPOINT_INFO_s *UdcEpInfo,
    UINT32                     DoubleBuffer,
    UINT32                     EndpointType,
    UINT32                     EndpointDir)
{
    UINT32 fifo_size;
    UINT32 physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;
    UINT32 max_pkt_size = Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;

    fifo_size = max_pkt_size / 4U;  // unit = 32bits

    if ((DoubleBuffer == 1U) && (flag_txfifo_double_buffer == 1U)) {
        fifo_size = fifo_size * 2U;
    } else if (EndpointType == UX_ISOCHRONOUS_ENDPOINT) {
        // for isochronous endpoints, double FIFO size is necessary
        fifo_size = fifo_size * 2U;
    } else {
        // do nothing
    }

    if (EndpointDir == USBD_EP_DIR_IN) {
        AmbaRTSL_UsbSetEpInEndPointType(physical_endpoint_index, EndpointType);
        // the stall might be set in early stage, clean it as now is a new beginning
        if (AmbaRTSL_UsbGetEpInStall(physical_endpoint_index) == 1U) {
            AmbaRTSL_UsbSetEpInStall(physical_endpoint_index, 0);
        }
        AmbaRTSL_UsbSetEpInCtrlF(physical_endpoint_index, 1);
        AmbaRTSL_UsbSetEpInNAK(physical_endpoint_index, 1); // set NAK initially
        AmbaRTSL_UsbSetEpInBuffsize(physical_endpoint_index, fifo_size);
        AmbaRTSL_UsbSetEpInMaxpksz(physical_endpoint_index, max_pkt_size);
        AmbaRTSL_UsbSetEpInDesptr(physical_endpoint_index, USB_UtilityVirDataDesc2PhyI32(UdcEpInfo->DmaDesc));
        AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);     // enable interrupt

        Udc->TxFifoUsed += (fifo_size * 4U);
        if (Udc->TxFifoUsed > (Udc->TxFifoMax - 512U)) {
            if (flag_txfifo_double_buffer == 0U) {
                // only error when double buffer is disabled.
                // print warning messages
                UINT32 LogEpAddr = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                                      "controller_endpoint_setup: TXFIFO size %d over max %d when creating endpoint 0x%X",
                                      Udc->TxFifoUsed,
                                      Udc->TxFifoMax,
                                      LogEpAddr,
                                      0, 0);

                usb_error_notify(USB_ERR_TXFIFO_OVERFLOW, 0);
            }
        }

        // if total size is too closed to maximum size, not use double buffer(default: used it).
        // Or data will be corrupted.
        if ((Udc->TxFifoUsed >= (Udc->TxFifoMax - 512U)) && (flag_txfifo_double_buffer == 1U)) {
            // Not expose the RTSL header (a12sdk) to customer , so extern here.
            UINT32 idx;
            for (idx = 1; idx < 5U; idx++) {
                if (AmbaRTSL_UsbGetEpInEndPointType(idx) == USBD_UDC_EP_BULK) {
                    // halved fifo size of this bulk endpoint
                    UINT32 size = AmbaRTSL_UsbGetEpInBuffsize(idx);
                    if (size != 0U) {
                        AmbaRTSL_UsbSetEpInBuffsize(idx, size / 2U);
                        // reverse used fifo size
                        Udc->TxFifoUsed -= (size / 2U) * 4U;
                    }
                }
            }
            flag_txfifo_double_buffer = 0;
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "controller_endpoint_setup: disable double buffer for bulk endpoint");
        }
    } else {
        fifo_size = max_pkt_size;
        AmbaRTSL_UsbSetEpOutType(physical_endpoint_index, EndpointType);
        AmbaRTSL_UsbSetEpOutRxReady(physical_endpoint_index, 1);                                              // receive ready
        AmbaRTSL_UsbSetEpOutNAK(physical_endpoint_index, 1);                                                  // set NAK initially
        AmbaRTSL_UsbSetEpOutMaxpksz(physical_endpoint_index, fifo_size);
        AmbaRTSL_UsbSetEpOutDesptr(physical_endpoint_index, USB_UtilityVirDataDesc2PhyI32(UdcEpInfo->DmaDesc)); // data buffer pointer
        AmbaRTSL_UsbEnOutEpInt(physical_endpoint_index);                                                      // enable interrupt
    }
}

static UINT32 endpoint_interval_get(const UX_SLAVE_ENDPOINT *pEndpoint)
{
    UINT32 interval = pEndpoint->ux_slave_endpoint_descriptor.bInterval;

    if (interval == 0U) {
        interval = 1;
    }
    if (interval > 16U) {
        interval = 16;
    }
    interval = interval - 1U;
    interval = 1UL << interval;
    return interval;
}

static UINT32 iso_request_queue_allocate(UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
#define USBD_ISO_REQ_QUEUE_MEM_SIZE (sizeof(UDC_ISO_REQUEST_s) * USBD_UDC_ISO_WRAPPER_NUM)

    static UINT8 iso_req_queue_mem[USBD_ISO_REQ_QUEUE_MEM_SIZE] __attribute__((section(".bss.noinit")));

    USB_UtilityMemorySet(iso_req_queue_mem, 0, sizeof(iso_req_queue_mem));

    if (USB_UtilityQueueCreate(&UdcEpInfo->IsoReqQueue,
                               iso_req_queue_mem,
                               sizeof(UDC_ISO_REQUEST_s),
                               USBD_UDC_ISO_WRAPPER_NUM) != USB_ERR_SUCCESS) {
        // Shall not be here.
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ISO request Queue Create Fail");
    }

    return USB_ERR_SUCCESS;
}

static UINT32 iso_request_queue_destroy(UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    UDC_ISO_REQUEST_s iso_request;

    USB_UtilityMemorySet(&iso_request, 0, sizeof(UDC_ISO_REQUEST_s));

    if (flag_iso_tx_ongoing != 0U) {
        // If there is Isochronous TX ongoing, we need to complete it
        // This is because Isochronous TX is asynchronous so no timeout if no In-Token comes.
        UX_SLAVE_TRANSFER       *tr;
        tr = iso_req_on_going.TransferRequest;
        if (tr != NULL) {
            tr->ux_slave_transfer_request_completion_code = UX_DCD_TRANSFER_ABORT;
            if (tr->completion_function != NULL) {
                tr->completion_function(tr);
            }
        }
        if (iso_req_on_going.DescWrapper != NULL) {
            iso_req_on_going.DescWrapper->Used = 0;
        }
        flag_iso_tx_ongoing = 0;
    }

    // empty queue and complete each transfer request inside queue
    while (USB_UtilityQueueIsoRecv(&UdcEpInfo->IsoReqQueue, &iso_request, USB_NO_WAIT) == USB_ERR_SUCCESS) {
        UX_SLAVE_TRANSFER *tr;

        tr                                            = iso_request.TransferRequest;
        if (tr != NULL) {
            tr->ux_slave_transfer_request_completion_code = UX_DCD_TRANSFER_ABORT;
            if (tr->completion_function != NULL) {
                tr->completion_function(tr);
            }
        }
        if (iso_request.DescWrapper != NULL) {
            iso_request.DescWrapper->Used = 0;
        }
    }

    if (USB_UtilityQueueDelete(&UdcEpInfo->IsoReqQueue) != USB_ERR_SUCCESS) {
        // Shall not be here.
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ISO request Queue Delete Fail");
    }

    return USB_ERR_SUCCESS;
}

static UINT32 iso_dma_desc_allocate(UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    USB_UtilityMemorySet(UdcEpInfo->IsoDescPool, 0, sizeof(UDC_ISO_DESC_INFO_s) * USBD_UDC_ISO_WRAPPER_NUM);
    if (USB_BufferIsoDataDescAttach(UdcEpInfo->IsoDescPool, USBD_UDC_ISO_WRAPPER_NUM) != USB_ERR_SUCCESS) {
        // shall not be here.
    }

    return USB_ERR_SUCCESS;
}

static UINT32 udc_iso_dma_desc_destroy(UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    // do nothing here.
    AmbaMisra_TouchUnused(UdcEpInfo);
    return USB_ERR_SUCCESS;
}

static void copy_iso_desc(UDC_DATA_DESC_s *pDescDestin, const UDC_DATA_DESC_s *pDescSource, UINT32 DescNumber)
{
    UINT32 i;

    for (i = 0; i < DescNumber; i++) {
        pDescDestin[i].Status  = pDescSource[i].Status;
        pDescDestin[i].DataPtr = pDescSource[i].DataPtr;

        if ((pDescDestin[i].Status & USBD_DMA_LAST_DESC) != 0U) {
            break;
        }
    }
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to patch DMA descriptor for Isochronous transfer.
 * */
UINT32 USBD_UdcIsoDescPatch(const USBD_UDC_s *pUdc, UDC_ENDPOINT_INFO_s *pUdcEpInfo, UINT32 MaxDescNum, UINT32 IsNewTransfer)
{
    const UDC_ISO_REQUEST_s *iso_req = &pUdcEpInfo->IsoReq;
    UINT32 uret;

    // Prepare transfer descriptor from the wrapper
    if (USB_UtilityQueueIsoRecv(&pUdcEpInfo->IsoReqQueue, iso_req, USB_NO_WAIT) == USB_ERR_SUCCESS) {
        UX_SLAVE_TRANSFER *transfer_request;
        UINT32 CurTime = tx_time_get();

        transfer_request = iso_req->TransferRequest;
        if (CurTime > iso_req->EndTimeStamp) {
            transfer_request->ux_slave_transfer_request_completion_code = UX_TRANSFER_TIMEOUT;
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcIsoDescPatch: iso request timeout");
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "    cur time: %d, end time : %d", CurTime, iso_req->EndTimeStamp, 0, 0, 0);
            if (transfer_request->completion_function != NULL) {
                transfer_request->completion_function(transfer_request);
            }
            iso_req->DescWrapper->Used = 0;
            uret                       = USB_ERR_FAIL;
        } else {
            UINT32 gap;
            UINT32 frame_number_cur;
            UINT32 frame_number     = pUdc->CurrentFrameNumber;
            UDC_DATA_DESC_s *pHead = pUdcEpInfo->DmaDesc;

            // invalidate endpoint DMA descriptor before modifying it.
            USB_UtilityCacheInvdDataDesc(pHead, MaxDescNum * sizeof(UDC_DATA_DESC_s));

            // copy descriptor from wrapper
            copy_iso_desc(pHead, iso_req->DescWrapper->DataDesc, MaxDescNum);

            // check time stamp
            frame_number_cur = AmbaRTSL_UsbGetDevStatusTs() & 0x7FFU;
            gap              = frame_number_cur - frame_number;
            if (gap > 1U) {
                // system loading may be high
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[Warning] USBD_UdcIsoDescPatch: system loading may be high, %d frame gap", gap, 0, 0, 0, 0);
            }

            if (IsNewTransfer != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USBD_UdcIsoDescPatch: start new transfer");
                frame_number = pUdc->CurrentFrameNumber;
            } else {
                frame_number = pUdcEpInfo->LastFrameNumber;
            }

            if ((pHead == NULL) && (MaxDescNum != 0U)) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcIsoDescPatch: Tried to invalidate illegal range");
                uret = USB_ERR_FAIL;
            } else {
                // update frame number field for each DMA descriptor
                const UX_SLAVE_ENDPOINT *usbx_endpoint = pUdcEpInfo->UxEndpoint;
                UINT32 interval                        = endpoint_interval_get(usbx_endpoint);
                UINT32 i;

                if (pHead != NULL) {
                    for (i = 0; i < MaxDescNum; i++) {
                        UDC_DATA_DESC_s *desc = &pHead[i];
                        UINT32 fnx = frame_number + (interval * (i + 1U));
                        fnx    = fnx & 0x7FFU;
                        fnx    = fnx << 16U;
                        desc->Status |= fnx;
                        if ((desc->Status & USBD_DMA_LAST_DESC) != 0U) {
                            pUdcEpInfo->LastFrameNumber = fnx >> 16U;
                            break;
                        }
                    }

                    USB_UtilityCacheFlushDataDesc(pHead, MaxDescNum * sizeof(UDC_DATA_DESC_s));

                    // mark Isochronous TX is ongoing, and
                    // save current request for later use (complete it when endpoint destroy)
                    flag_iso_tx_ongoing = 1;
                    iso_req_on_going = *iso_req;

                    uret = USB_ERR_SUCCESS;
                } else {
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcIsoDescPatch(): pHead is NULL.");
                    uret = USB_ERR_FAIL;
                }
            }
        }
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBD_UdcIsoDescPatch: no more iso request, IsNewTransfer = %d", IsNewTransfer, 0, 0, 0, 0);
        uret = USB_ERR_FAIL;
    }

    return uret;
}
/** @} */

static UINT32 dma_descriptor_allocate(UDC_ENDPOINT_INFO_s *UdcEpInfo, UINT32 LogAddr, UINT32 Count)
{
    UINT32 uret;

    AmbaRTSL_UsbSetEp20LogicalID(UdcEpInfo->UdcIndex, LogAddr & 0x07FU);

    // create DMA descriptor if NULL
    if (UdcEpInfo->DmaDesc == NULL) {
        UdcEpInfo->DmaDescSize = sizeof(UDC_DATA_DESC_s) * Count;

        if (UdcEpInfo->Direction == USBD_EP_DIR_OUT) {
            if (Count > UBUF_NUM_UDC_DESC_OUT) {
                uret = UX_MEMORY_INSUFFICIENT;
            } else {
                uret = USB_BufferDataDescAttach(UBUF_UDC_BULK_DESC_OUT, &UdcEpInfo->DmaDesc);
            }
        } else {
            if (Count > UBUF_NUM_UDC_DESC_IN) {
                uret = UX_MEMORY_INSUFFICIENT;
            } else {
                uret = USB_BufferDataDescAttach(UBUF_UDC_BULK_DESC_IN, &UdcEpInfo->DmaDesc);
            }
        }

        if (uret == USB_ERR_SUCCESS) {
            UdcEpInfo->DmaDescNum = Count;
        } else {
            UdcEpInfo->DmaDescSize = 0;
            UdcEpInfo->DmaDescNum  = 0;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dma_descriptor_allocate: can't attach DMA desc for EP 0x%X.", LogAddr, uret, 0, 0, 0);
        }
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dma_descriptor_allocate: DMA desc for EP 0x%X has been attached.", LogAddr, 0, 0, 0, 0);
        uret = USB_ERR_SUCCESS;
    }
    USBD_UdcInitDmaDesc(UdcEpInfo->DmaDesc, Count);
    return uret;
}

static void udc_dma_descriptor_destroy(USBD_UDC_s *Udc, UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    UINT32 physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;
    UINT32 desc_num = UdcEpInfo->DmaDescNum;
    UINT32 i;
    UINT32 fifo_size;

    if (UdcEpInfo->Direction == USBD_EP_DIR_IN) {
        // endpoint IN
        AmbaRTSL_UsbDisInEpInt(physical_endpoint_index);
        AmbaRTSL_UsbSetEpInCtrlF(physical_endpoint_index, 1);
        fifo_size        = (AmbaRTSL_UsbGetEpInBfszSize(physical_endpoint_index) * 4U);
        Udc->TxFifoUsed -= fifo_size;
    } else {
        // endpoint OUT
        AmbaRTSL_UsbDisOutEpInt(physical_endpoint_index);
    }

    if (UdcEpInfo->DmaDesc == NULL) {
        // null pointer handling
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "udc_dma_descriptor_destroy: DESC is NULL");
    } else {
        for (i = 0; i < desc_num; i++) {
            UdcEpInfo->DmaDesc[i].Status = USBD_DMA_HOST_NOT_RDY;
        }

        USB_UtilityCacheFlushDataDesc(UdcEpInfo->DmaDesc, UdcEpInfo->DmaDescSize);

        if (UdcEpInfo->Direction == USBD_EP_DIR_OUT) {
            USB_BufferDataDescDetach(UBUF_UDC_BULK_DESC_OUT, UdcEpInfo->DmaDesc);
        } else {
            USB_BufferDataDescDetach(UBUF_UDC_BULK_DESC_IN, UdcEpInfo->DmaDesc);
        }

        UdcEpInfo->DmaDesc = NULL;
    }
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to acquire mutex before update RX state.
 * */
void USBD_UdcRxStateMutexGet(UINT32 Timeout)
{
    if ((flag_udc_init & USBD_FLAG_USBD_RX_STATE_MUTEX_INIT) != 0U) {
        if (USB_UtilityMutexTake(&local_usbd_udc.RxStateMutex, Timeout) != USB_ERR_SUCCESS) {
            // shall not be here.
        }
    }
}
/**
 * Called by driver to release mutex after update RX state.
 * */
void USBD_UdcRxStateMutexPut(void)
{
    if ((flag_udc_init & USBD_FLAG_USBD_RX_STATE_MUTEX_INIT) != 0U) {
        if (USB_UtilityMutexGive(&local_usbd_udc.RxStateMutex) != USB_ERR_SUCCESS) {
            // shall not be here.
        }
    }
}
/**
 * Called by driver to allocate buffer for one UX endpoint.
 * */
void USBD_UdcEndpointBufferAllocate(UX_SLAVE_ENDPOINT *Endpoint)
{
    UINT32 uret;
    UINT8 *buffer_ptr;
    UINT32 buffer_size;
    UINT32 ux_ep_type = udc_get_endpoint_type(Endpoint);

    switch (ux_ep_type) {
    case UX_CONTROL_ENDPOINT:
        uret = USB_BufferU8Attach(UBUF_USBX_CTRL, &buffer_ptr);
        if (uret == USB_ERR_SUCCESS) {
            buffer_size = USB_BufferSizeGet(UBUF_USBX_CTRL);
        } else {
            buffer_size = 0;
        }
        break;
    case UX_BULK_ENDPOINT_IN:
    case UX_INTERRUPT_ENDPOINT_IN:
        uret = USB_BufferU8Attach(UBUF_USBX_BULK_IN, &buffer_ptr);
        if (uret == USB_ERR_SUCCESS) {
            buffer_size = USB_BufferSizeGet(UBUF_USBX_BULK_IN);
            (void)uxd_ep_in_max_dlen_set(buffer_size);
        } else {
            buffer_size = 0;
        }
        break;
    case UX_BULK_ENDPOINT_OUT:
    case UX_INTERRUPT_ENDPOINT_OUT:
        uret = USB_BufferU8Attach(UBUF_USBX_BULK_OUT, &buffer_ptr);
        if (uret == USB_ERR_SUCCESS) {
            buffer_size = USB_BufferSizeGet(UBUF_USBX_BULK_OUT);
            (void)uxd_ep_out_max_dlen_set(buffer_size);
        } else {
            buffer_size = 0;
        }
        break;
    case UX_ISOCHRONOUS_ENDPOINT_IN: {
        UINT32 desc_number = (USBD_EP_ISO_MAX_BUFFER_PER_XFER) / (Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize & 0x7FFU);

        uret = USB_ERR_SUCCESS;
        // Won't attach the data buffer because it's provided by the application.
        // The driver only decides the maximum buffer size.
        buffer_size = Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize & 0x7FFU;
        buffer_size = desc_number * buffer_size;
        buffer_ptr  = NULL;
    }
    break;
    default:
        buffer_ptr  = NULL;
        buffer_size = 0;
        uret        = USB_ERR_FAIL;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] EP type 0x%x IS NOT IMPLEMENT!", ux_ep_type, 0, 0, 0, 0);
        break;
    }

    if (uret == USB_ERR_SUCCESS) {
        UX_SLAVE_TRANSFER *transfer_request;
        transfer_request                                         = &Endpoint->ux_slave_endpoint_transfer_request;
        transfer_request->ux_slave_transfer_request_data_pointer = buffer_ptr;
        transfer_request->max_buffer_size                        = buffer_size;
        // Create endpoint-specified semaphore.
        if (USB_UtilitySemaphoreCreate(&transfer_request->ux_slave_transfer_request_semaphore, 0) != USB_ERR_SUCCESS) {
            // error handling
            UINT32 LogAddr        = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] Failed to create semaphore for endpoint 0x%x", LogAddr, 0, 0, 0, 0);
        }
    } else {
        // error handling
        UINT32 LogAddr        = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] Failed to allocate buffer for endpoint 0x%x", LogAddr, 0, 0, 0, 0);
    }
}
/**
 * Called by driver to release buffer for one UX endpoint.
 * */
void USBD_UdcEndpointBufferDestroy(UX_SLAVE_ENDPOINT *Endpoint)
{
    UX_SLAVE_TRANSFER *transfer_request = &Endpoint->ux_slave_endpoint_transfer_request;
    const UINT8 *buffer_ptr             = transfer_request->ux_slave_transfer_request_data_pointer;
    UINT32 ux_ep_type                   = udc_get_endpoint_type(Endpoint);

    switch (ux_ep_type) {
    case UX_CONTROL_ENDPOINT:
        USB_BufferU8Detach(UBUF_USBX_CTRL, buffer_ptr);
        break;
    case UX_BULK_ENDPOINT_IN:
    case UX_INTERRUPT_ENDPOINT_IN:
        USB_BufferU8Detach(UBUF_USBX_BULK_IN, buffer_ptr);
        break;
    case UX_BULK_ENDPOINT_OUT:
    case UX_INTERRUPT_ENDPOINT_OUT:
        USB_BufferU8Detach(UBUF_USBX_BULK_OUT, buffer_ptr);
        break;
    case UX_ISOCHRONOUS_ENDPOINT_IN:
        // do nothting here.
        break;
    default: {
        // no action
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer destroy] EP Mask 0x%x IS NOT IMPLEMENT!", ux_ep_type, 0, 0, 0, 0);
        break;
    }
    }

    transfer_request->ux_slave_transfer_request_data_pointer = NULL;

    if (USB_UtilitySemaphoreDelete(&transfer_request->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
        // error handling
        UINT32 logical_address = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] Failed to delete semaphore for endpoint 0x%x", logical_address, 0, 0, 0, 0);
    }

    // reset the semaphore.
    USB_UtilityMemorySet(&transfer_request->ux_slave_transfer_request_semaphore, 0, sizeof(AMBA_KAL_SEMAPHORE_t));
}


static UINT32 udc_endpoint_direction_get(UINT32 LogAddr)
{
    UINT32 endpoint_direction;

    if ((LogAddr & 0x80U) != 0U) {
        endpoint_direction = USBD_EP_DIR_IN;
    } else {
        endpoint_direction = USBD_EP_DIR_OUT;
    }
    return endpoint_direction;
}

static UINT32 udc_endpoint_assign(UDC_ENDPOINT_MAP_INFO_s *MapInfo, UINT32 PhysicalEndpointIndex, UINT32 LogAddr, UINT32 UdcEndpointDirection, UINT32 MaxUdc20Index)
{
    UINT8 i;
    UINT32 uret = USB_ERR_NO_PHY_ENDPOINT_AVAILABLE;

    // asign a free entry.
    for (i = 1; i < UDC_MAX_ENDPOINT_NUM; i++) {
        UDC_ENDPOINT_MAP_s *map = &(MapInfo->EndpointMap[i]);
        if (map->Used == 0U) {
            map->PhysicalIndex = PhysicalEndpointIndex; // force to have the same index of logical address.
            map->LogicalAddr   = LogAddr;
            map->Direction     = UdcEndpointDirection;
            map->Udc20Index    = MaxUdc20Index;
            map->Used          = 1;
            uret               = USB_ERR_SUCCESS;
            break;
        }
    }

    if (i == UDC_MAX_ENDPOINT_NUM) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcEndpointMapCreate: Reach maximum endpoint number.");
        uret = USB_ERR_NO_PHY_ENDPOINT_AVAILABLE;
    }

    return uret;
}

/**
 * Called by driver to create endpoint map (logical/physical/udc20 mapping)
 * */
UINT32 USBD_UdcEndpointMapCreate(USBD_UDC_s *Udc, UINT32 LogAddr)
{
    UDC_ENDPOINT_MAP_INFO_s *map_info = &Udc->EndpointMapInfo;
    UINT8 i;
    UINT32 endpoint_direction;
    UINT32 max_endpoint_index      = 0;
    UINT32 physical_endpoint_index = LogAddr & 0x07FU;
    UINT32 max_udc20_index = 0;
    UINT32 flag_leave  = 0;
    UINT32 uret        = USB_ERR_SUCCESS;

    endpoint_direction = udc_endpoint_direction_get(LogAddr);

    if (LogAddr != 0U) {
        max_endpoint_index = 1;
    }

    if (physical_endpoint_index >= 0x06U) {
        // print warning messages
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "------------ ERROR ----------------------------------------");
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, " USB Endpoint point index %d can't exceed 6!", LogAddr, 0, 0, 0, 0);
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "-----------------------------------------------------------");
    }

    // According to test, it seems that we can only support
    // Logical Endpoint Address 0x1 ~ 0x5 and 0x81 ~ 0x85
    // And they map to Physical Endpoint Out 0x01 ~ 0x05 and In 0x01 ~ 0x05.
    // And don't touch Physical Endpoint 0x0.

    // scan whole map to get information first.
    for (i = 0; i < UDC_MAX_ENDPOINT_NUM; i++) {
        const UDC_ENDPOINT_MAP_s *map = &(map_info->EndpointMap[i]);
        if (map->Used == 1U) {
            if (map->Direction == endpoint_direction) {
                if (map->LogicalAddr == LogAddr) {
                    // Should not process here for control EP, because EndpointMap[0] will be extracted from endpoint0_map
                    if (LogAddr == 0U) {
                        uret = USB_ERR_SUCCESS;
                    } else {
                        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_UdcEndpointMapCreate: Logical Endpoint 0x%X is already created.",
                                              LogAddr, 0, 0, 0, 0);
                        uret = USB_ERR_INVALID_ENDPOINT_ADDRESS;
                    }
                    flag_leave = 1;
                } else {
                    if (max_endpoint_index <= map->PhysicalIndex) {
                        max_endpoint_index = map->PhysicalIndex + 1U;
                    }
                }
            }

            if (flag_leave == 0U) {
                if (max_udc20_index <= map->Udc20Index) {
                    max_udc20_index = map->Udc20Index + 1U;
                }
            } else {
                break;
            }
        }
    }


    if (flag_leave == 0U) {
        // asign a free entry.
        uret = udc_endpoint_assign(map_info, physical_endpoint_index, LogAddr, endpoint_direction, max_udc20_index);
    }

    return uret;
}
/** @} */

static void udc_endpoint_map_destroy(USBD_UDC_s *Udc, UINT32 LogAddr)
{
    UDC_ENDPOINT_MAP_INFO_s *map_info = &Udc->EndpointMapInfo;
    UINT8 i;

    for (i = 0; i < UDC_MAX_ENDPOINT_NUM; i++) {
        UDC_ENDPOINT_MAP_s *map = &(map_info->EndpointMap[i]);
        if ((map->LogicalAddr == LogAddr) && (map->Used != 0U)) {
            // Should not clear EndpointMap[0] status, because it may cause wrong
            // mapping in next time if the first mapping is not for control endpoint.
            if (LogAddr != 0U) {
                map->Used = 0;
                break;
            }
        }
    }
}

static void endpoint_attribute_setup_1(const UX_SLAVE_ENDPOINT *UsbxEndpoint,
                                       UDC_ENDPOINT_INFO_s *UdcEpInfo,
                                       UINT32 UxEpType,
                                       UINT32 *UdcEpType,
                                       UINT32 *UdcEpDirection,
                                       UINT32 *FlagDoubleBuffer,
                                       UINT32 *DescNumber)
{

    switch (UxEpType) {
    case UX_CONTROL_ENDPOINT:

        *UdcEpType    = USBD_UDC_EP_CONTROL;
        *UdcEpDirection     = 0;                                           // NO MATTER
        UdcEpInfo->Direction = USBD_EP_DIR_IN;

        AmbaRTSL_UsbSetEp20(0, 0);                               // reset
        AmbaRTSL_UsbSetEp20EndPointType(0, UX_CONTROL_ENDPOINT); // control type
        AmbaRTSL_UsbSetEp20EndPointDir(0, 1);                    // FIXME: IN direction or OUT, no matter for control endpoint #0 ?
        AmbaRTSL_UsbSetEp20LogicalID(0, 0);
        if (USBD_UdcEnumSpeedGet() == USB_CONNECT_SPEED_HIGH) {
            AmbaRTSL_UsbSetEp20MaxPktSize(0, 64);
        } else {
            AmbaRTSL_UsbSetEp20MaxPktSize(0, 16);
        }
        // After Reset, we need to enable rde for next setup packet
        AmbaRTSL_UsbSetDevCtlRde(1); // enable RX DMA

        break;

    case UX_BULK_ENDPOINT_IN:
        *UdcEpType         = USBD_UDC_EP_BULK;
        *UdcEpDirection    = USBD_EP_DIR_IN;
        UdcEpInfo->Direction   = USBD_EP_DIR_IN;
        *FlagDoubleBuffer  = 1;
        *DescNumber        = USBD_BULK_IN_CHAIN_DESC_NUM;
        break;

    case UX_BULK_ENDPOINT_OUT:
        *UdcEpType        = USBD_UDC_EP_BULK;
        *UdcEpDirection   = USBD_EP_DIR_OUT;
        UdcEpInfo->Direction  = USBD_EP_DIR_OUT;
        *DescNumber       = USBD_BULK_OUT_CHAIN_DESC_NUM;
        break;

    case UX_INTERRUPT_ENDPOINT_IN:
        *UdcEpType        = USBD_UDC_EP_INT;
        *UdcEpDirection   = USBD_EP_DIR_IN;
        UdcEpInfo->Direction  = USBD_EP_DIR_IN;
        *DescNumber       = USBD_INTERRUPT_CHAIN_DESC_NUM;
        break;

    case UX_INTERRUPT_ENDPOINT_OUT:
        *UdcEpType       = USBD_UDC_EP_INT;
        *UdcEpDirection  = USBD_EP_DIR_OUT;
        UdcEpInfo->Direction = USBD_EP_DIR_OUT;
        *DescNumber      = USBD_INTERRUPT_CHAIN_DESC_NUM;
        break;

    case UX_ISOCHRONOUS_ENDPOINT_IN:
        *UdcEpType         = USBD_UDC_EP_ISO;
        *UdcEpDirection    = USBD_EP_DIR_IN;
        UdcEpInfo->Direction   = USBD_EP_DIR_IN;
        *FlagDoubleBuffer = 1;
        *DescNumber   = (USBD_EP_ISO_MAX_BUFFER_PER_XFER) / (UsbxEndpoint->ux_slave_endpoint_descriptor.wMaxPacketSize & 0x7FFU);
        break;

    case UX_ISOCHRONOUS_ENDPOINT_OUT:
        *UdcEpType     = USBD_UDC_EP_ISO;
        *UdcEpDirection      = USBD_EP_DIR_OUT;
        UdcEpInfo->Direction  = USBD_EP_DIR_OUT;
        *DescNumber  = USBD_ISOCHRONOUS_CHAIN_DESC_NUM;
        break;

    default:
        // unsupport usb type.
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_UdcDmaMemoryAllocate:Unsupport USB type 0x%x", UxEpType, 0, 0, 0, 0);
        break;
    }
    return;
}

/**
 * Called by driver to allocate DMA descriptor buffer for one endpoint.
 * */
void USBD_UdcDmaMemoryAllocate(USBD_UDC_s *Udc, UX_SLAVE_ENDPOINT *UsbxEndpoint)
{
    UINT32 ep_type      = 0;
    UINT32 ep_direction = 0;
    UINT32 ux_ep_type;
    UDC_ENDPOINT_INFO_s *udc_ep_info;
    UINT32 logical_ep_address = UsbxEndpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
    const UDC_ENDPOINT_MAP_s *endpoint_map;
    UINT32 flag_double_buffer = 0;
    UINT32 desc_number   = 0;

    endpoint_map = USBD_UdcEndpointMapGet(Udc, logical_ep_address);
    if (endpoint_map != NULL) {
        // Fetch the address of the physical endpoint.
        udc_ep_info = &Udc->DcdEndpointsInfoArray[endpoint_map->Udc20Index];

        udc_ep_info->EpState |= USBD_UDC_ED_STATUS_USED;

        // Keep the physical endpoint address in the endpoint container.
        UsbxEndpoint->ux_slave_endpoint_ed = USB_UtilityUdcEd2VoidP(udc_ep_info);

        // And its mask.
        udc_ep_info->PhysicalEpIndex = endpoint_map->PhysicalIndex;
        udc_ep_info->UdcIndex        = endpoint_map->Udc20Index;

        // Save the endpoint pointer.
        udc_ep_info->UxEndpoint = UsbxEndpoint;

        // Build the endpoint mask from the endpoint descriptor.
        ux_ep_type = udc_get_endpoint_type(UsbxEndpoint);

        endpoint_attribute_setup_1(UsbxEndpoint, udc_ep_info, ux_ep_type, &ep_type, &ep_direction, &flag_double_buffer, &desc_number);

        if (ux_ep_type != (UINT32)UX_CONTROL_ENDPOINT) {
            (void)dma_descriptor_allocate(udc_ep_info, logical_ep_address, desc_number);
            if (ep_direction == USBD_EP_DIR_OUT) {
                (void)rx_swfifo_allocate(Udc, udc_ep_info, logical_ep_address);
            }
        }

        if (ux_ep_type == (UINT32)UX_ISOCHRONOUS_ENDPOINT_IN) {
            if (iso_request_queue_allocate(udc_ep_info) == USB_ERR_SUCCESS) {
                (void)iso_dma_desc_allocate(udc_ep_info);
            }
        }

        // Reset the endpoint.
        (void)USBD_DrvEndpointReset(Udc, UsbxEndpoint);

        if (ux_ep_type != (UINT32)UX_CONTROL_ENDPOINT) {
            // setup controller endpoint registers other than Endpoint 0
            controller_endpoint_setup(Udc, UsbxEndpoint, udc_ep_info, flag_double_buffer, ep_type, ep_direction);
        }

        // Enable this endpoint.

        // A9's way to setup and enable this endpoint
        // setup Control endpoint register
        AmbaRTSL_UsbSetEp20EndPointType(udc_ep_info->UdcIndex, ep_type);  // control type
        AmbaRTSL_UsbSetEp20EndPointDir(udc_ep_info->UdcIndex, ep_direction);

        AmbaRTSL_UsbSetEp20MaxPktSize(udc_ep_info->UdcIndex, UsbxEndpoint->ux_slave_endpoint_descriptor.wMaxPacketSize);
        AmbaRTSL_UsbSetEp20ConfigID(udc_ep_info->UdcIndex, AmbaRTSL_UsbGetDevStatusCfg());
        AmbaRTSL_UsbSetEp20Interface(udc_ep_info->UdcIndex, AmbaRTSL_UsbGetDevStatusIntf());
        AmbaRTSL_UsbSetEp20AltSetting(udc_ep_info->UdcIndex, AmbaRTSL_UsbGetDevStatusAlt());
    }
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to release DMA descriptor buffer for one endpoint.
 * */
void USBD_UdcDmaMemoryDestroy(USBD_UDC_s *Udc, const UX_SLAVE_ENDPOINT *UsbxEndpoint)
{
    UDC_ENDPOINT_INFO_s *udc_ep_info;
    UINT32 ux_ep_type;
    UINT32 logical_ep_address = UsbxEndpoint->ux_slave_endpoint_descriptor.bEndpointAddress;

    // Keep the physical endpoint address in the endpoint container.
    udc_ep_info = USB_UtilityVoidP2UdcEd(UsbxEndpoint->ux_slave_endpoint_ed);

    // Build the endpoint type from the endpoint descriptor.
    ux_ep_type = udc_get_endpoint_type(UsbxEndpoint);

    // Turn off the endpoint enable flag.
    switch (ux_ep_type) {
    case UX_CONTROL_ENDPOINT:
        // put all descriptor as not ready
        // to prevent from DMA engine confused

        ctrl_ep_desc.CtrlInDataDesc.Status   = USBD_DMA_HOST_NOT_RDY;
        ctrl_ep_desc.CtrlOutDataDesc.Status  = USBD_DMA_HOST_NOT_RDY;
        ctrl_ep_desc.CtrlOutSetupDesc.Status = USBD_DMA_HOST_NOT_RDY;

        USB_UtilityCacheFlushDataDesc(&ctrl_ep_desc.CtrlInDataDesc, sizeof(UDC_DATA_DESC_s));
        USB_UtilityCacheFlushDataDesc(&ctrl_ep_desc.CtrlOutDataDesc, sizeof(UDC_DATA_DESC_s));
        USB_UtilityCacheFlushSetupDesc(&ctrl_ep_desc.CtrlOutSetupDesc, sizeof(UDC_SETUP_DESC_s));

        break;
    case UX_BULK_ENDPOINT_IN:
    case UX_BULK_ENDPOINT_OUT:
    case UX_INTERRUPT_ENDPOINT_IN:
    case UX_INTERRUPT_ENDPOINT_OUT:
    case UX_ISOCHRONOUS_ENDPOINT_IN:
        udc_dma_descriptor_destroy(Udc, udc_ep_info);
        rx_fifo_destroy(udc_ep_info);
        break;
    default:
        // not supported
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_destroy] EP 0x%x IS NOT IMPLEMENT!", ux_ep_type, 0, 0, 0, 0);
        break;
    }

    if (ux_ep_type == (UINT32)UX_ISOCHRONOUS_ENDPOINT_IN) {
        // in case Isochronous TX is on going, need to stop DMA by clear poll-demand bit
        // otherwise BNA occurs at next IN-TOKEN with poll-demand bit set.
        AmbaRTSL_UsbSetEpInPollDemand(udc_ep_info->PhysicalEpIndex, 0);
        (void)iso_request_queue_destroy(udc_ep_info);
        (void)udc_iso_dma_desc_destroy(udc_ep_info);
    }

    udc_endpoint_map_destroy(Udc, logical_ep_address);

    // disable this endpoint
    AmbaRTSL_UsbSetEp20(udc_ep_info->UdcIndex, 0);  // reset

    // We can free this endpoint.
    USB_UtilityMemorySet(udc_ep_info, 0, sizeof(UDC_ENDPOINT_INFO_s));
    udc_ep_info->EpState = USBD_UDC_ED_STATUS_UNUSED;
}

/**
 * Called by driver to get the last DMA descriptor of the chain.
 * */
UDC_DATA_DESC_s* USBD_UdcLastDescGet(UDC_DATA_DESC_s* Head, UINT32 MaxDescNum)
{
    UDC_DATA_DESC_s *desc = NULL;
    UINT32 i              = 0;

    if (Head != NULL) {
        USB_UtilityCacheInvdDataDesc(Head, MaxDescNum * sizeof(UDC_DATA_DESC_s));

        for (i = 0; i < MaxDescNum; i++) {
            desc = &Head[i];
            if ((desc->Status & USBD_DMA_LAST_DESC) != 0U) {
                break;
            }
        }

        if (i == MaxDescNum) {
            desc = NULL;
        }
    } else {
        desc = NULL;
    }

    return desc;
}

/**
 * Called by driver to initialize DMA descriptor for receiving setup packet. DMA descripor is set as "READY".
 * */
void USBD_UdcInitCtrlSetupDesc(void)
{
    ctrl_ep_desc.CtrlOutSetupDesc.Status = 0;
    ctrl_ep_desc.CtrlOutSetupDesc.Data0  = 0xFFFFFFFFU;
    ctrl_ep_desc.CtrlOutSetupDesc.Data1  = 0xFFFFFFFFU;

    USB_UtilityCacheFlushSetupDesc(&ctrl_ep_desc.CtrlOutSetupDesc, sizeof(UDC_SETUP_DESC_s));
}

/**
 * Called by driver to obtain the DMA descriptor for control-out transfer
 * */
void USBD_UdcCtrlOutDataDescGet(UDC_DATA_DESC_s **Desc)
{
    *Desc = &ctrl_ep_desc.CtrlOutDataDesc;
}

/**
 * Called by driver to obtain the DMA descriptor for control-in transfer
 * */
void USBD_UdcCtrlInDataDescGet(UDC_DATA_DESC_s **Desc)
{
    *Desc = &ctrl_ep_desc.CtrlInDataDesc;
}

/**
 * Called by driver to reset DMA descriptor for non-control transfer. DMA descripor is set as "NOT-READY".
 * */
void USBD_UdcDmaDescReset(const UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    UDC_DATA_DESC_s *desc = UdcEpInfo->DmaDesc;

    if (desc != NULL) {
        UINT32 i;

        USB_UtilityCacheInvdDataDesc(desc, UdcEpInfo->DmaDescSize);

        for (i = 0; i < UdcEpInfo->DmaDescNum; i++) {
            desc[i].Status = USBD_DMA_HOST_NOT_RDY;
        }

        USB_UtilityCacheFlushDataDesc(desc, UdcEpInfo->DmaDescSize);
    }
}

/**
 * Called by driver to initialize DMA descriptor for control-in/control-out transfers. DMA descripor is set as "NOT-READY".
 * */
void USBD_UdcInitCtrlDataDesc(void)
{
    UINT8 *buffer_ptr;

    if (USB_BufferU8Attach(UBUF_UDC_CTRL_OUT, &buffer_ptr) == USB_ERR_SUCCESS) {
        ctrl_ep_desc.CtrlOutDataDesc.Status   = USBD_DMA_HOST_NOT_RDY | USBD_DMA_LAST_DESC;
        ctrl_ep_desc.CtrlOutDataDesc.Reserved = 0xFFFFFFFFU;
        ctrl_ep_desc.CtrlOutDataDesc.DataPtr  = USB_UtilityVirP8ToPhyI32(buffer_ptr);
        // chain next descriptor to the self -> single descriptor
        ctrl_ep_desc.CtrlOutDataDesc.NextDescPtr = USB_UtilityVirDataDesc2PhyI32(&ctrl_ep_desc.CtrlOutDataDesc);
        USB_UtilityCacheFlushDataDesc(&ctrl_ep_desc.CtrlOutDataDesc, sizeof(UDC_DATA_DESC_s));
    }

    if (USB_BufferU8Attach(UBUF_UDC_CTRL_IN, &buffer_ptr) == USB_ERR_SUCCESS) {
        ctrl_ep_desc.CtrlInDataDesc.Status   = USBD_DMA_HOST_NOT_RDY | USBD_DMA_LAST_DESC;
        ctrl_ep_desc.CtrlInDataDesc.Reserved = 0xFFFFFFFFU;
        ctrl_ep_desc.CtrlInDataDesc.DataPtr  = USB_UtilityVirP8ToPhyI32(buffer_ptr);
        // chain next descriptor to the self -> single descriptor
        ctrl_ep_desc.CtrlInDataDesc.NextDescPtr = USB_UtilityVirDataDesc2PhyI32(&ctrl_ep_desc.CtrlInDataDesc);
        USB_UtilityCacheFlushDataDesc(&ctrl_ep_desc.CtrlInDataDesc, sizeof(UDC_DATA_DESC_s));
    }
}

/**
 * Called by driver to initialize DMA descriptor for non-control transfers. DMA descripor is set as "NOT-READY".
 * */
void USBD_UdcInitDmaDesc(UDC_DATA_DESC_s *Desc, UINT32 Number)
{
    UINT32 i;

    if (Number != 0U) {
        if (Desc != NULL) {
            USB_UtilityCacheInvdDataDesc(Desc, Number * sizeof(UDC_DATA_DESC_s));

            if (Number > 1U) {
                for (i = 0; i < (Number - 1U); i++) {
                    const UDC_DATA_DESC_s *Next = &(Desc[i + 1U]);
                    Desc[i].Status      = USBD_DMA_HOST_NOT_RDY;
                    Desc[i].Reserved    = 0xffffffffU;
                    Desc[i].DataPtr     = 0xffffffffU;
                    Desc[i].NextDescPtr = USB_UtilityVirDataDesc2PhyI32(Next);
                }
                Desc[Number - 1U].NextDescPtr = USB_UtilityVirDataDesc2PhyI32(&Desc[0]);
            } else {
                Desc[0].Status      = USBD_DMA_HOST_NOT_RDY;
                Desc[0].Reserved    = 0xffffffffU;
                Desc[0].DataPtr     = 0xffffffffU;
                Desc[0].NextDescPtr = USB_UtilityVirDataDesc2PhyI32(Desc);
            }

            USB_UtilityCacheFlushDataDesc(Desc, Number * sizeof(UDC_DATA_DESC_s));
        } else {
            // null pointer handling
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcInitDmaDesc: Tried to init illegal desc");
        }
    }
}
/**
 * ???
 * */
void USBD_UdcRxSwfifoPush(UDC_ENDPOINT_INFO_s *UdcEpInfo, const UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 ux_ep_type;
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UDC_RDMA_SUB_DESC_INFO_s *sub_dma_info;

    rx_mutex_get(USB_WAIT_FOREVER);

    usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
    ux_ep_type    = udc_get_endpoint_type(usbx_endpoint);

    sub_dma_info = &UdcEpInfo->SubDmaInfo;

    if ((UdcEpInfo->RxDmaState & UDC_RX_DMA_STAT_ALT_DESC) != 0U) {
        if (udc_is_control_endpoint_type(ux_ep_type) == 0U) {
            // The dummpy desc has completed.
            USB_UtilityCacheInvdDataDesc(&sub_dma_info->DmaDesc, sizeof(UDC_DATA_DESC_s));
            if ((sub_dma_info->DmaDesc.Status & USBD_DMA_DONE) != 0U) {
                UINT32 i;
                UDC_RX_SWFIFO_INFO_s *Info = NULL;

                // Find the available list from the pool.
                for (i = 0; i < UDC_RX_SWFIFO_DEPTH; i++) {
                    Info = &UdcEpInfo->RxSwFifoInfo[i];
                    if (Info->List.container == NULL) {
                        break;
                    }
                }
                if (i == UDC_RX_SWFIFO_DEPTH) {
                    // rxfifo full handling
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcRxSwfifoPush: no available rx fifo info.");
                } else {
                    // Check whether the list is empty to decide the base address.
                    if (udc_rxfifo_list_empty(&UdcEpInfo->RxSwFifoList) != 0) {
                        Info->Addr = UdcEpInfo->RxSwFifoBase;
                    } else {
                        const UDC_RX_SWFIFO_INFO_s *last = udc_rxfifo_list_get_last_entry(&UdcEpInfo->RxSwFifoList);
                        Info->Addr = &last->Addr[last->Length];
                    }
                    Info->Length = sub_dma_info->DmaDesc.Status & 0xFFFFU;
                    if (Info->Length != 0U) {
                        USB_UtilityCacheInvdUInt8(sub_dma_info->Buffer, Info->Length);
                        USB_UtilityMemoryCopy(Info->Addr, sub_dma_info->Buffer, Info->Length);
                    }
                    // Add the list to the tail.
                    udc_rxfifo_list_add_tail(&Info->List, &UdcEpInfo->RxSwFifoList, Info);
                }
                // Tag the Sub descriptor as not ready.
                sub_dma_info->DmaDesc.Status = USBD_DMA_HOST_NOT_RDY;
                USB_UtilityCacheFlushDataDesc(&sub_dma_info->DmaDesc, sizeof(UDC_DATA_DESC_s));
            }
        }
    }
    rx_mutex_put();
}

/**
 * ???
 * */
void USBD_UdcRxSwfifoPop(const UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT8 **DataPtr, UINT32 *Length)
{
    UINT32 ux_ep_type;
    const UX_SLAVE_ENDPOINT *usbx_endpoint;

    rx_mutex_get(USB_WAIT_FOREVER);

    usbx_endpoint   = TransferRequest->ux_slave_transfer_request_endpoint;
    ux_ep_type      = udc_get_endpoint_type(usbx_endpoint);

    *DataPtr = TransferRequest->ux_slave_transfer_request_current_data_pointer;
    *Length  = TransferRequest->ux_slave_transfer_request_requested_length -
               TransferRequest->ux_slave_transfer_request_actual_length;

    if (udc_is_control_endpoint_type(ux_ep_type) == 0U) {
        if (udc_rxfifo_list_empty(&UdcEpInfo->RxSwFifoList) == 0) {
            UINT32 data_size;
            UINT32 max_pkt_size;
            UINT32 total_length         = 0;
            UINT8 *tmp_data_ptr         = *DataPtr;
            UINT32 flag_short_packet    = 0;
            UINT32 max_data_size;
            UDC_RX_SWFIFO_INFO_s *rx_swfifo_info  = NULL;

            max_pkt_size  = udc_get_endpoint_max_packet_size(usbx_endpoint);
            max_data_size = UdcEpInfo->DmaDescNum * max_pkt_size;
            if (*Length > max_data_size) {
                data_size = max_data_size;
            } else {
                data_size = *Length;
            }

            // Traverse all queue list in RxSwFifoList. First in, first out.
            while (udc_rxfifo_list_empty(&UdcEpInfo->RxSwFifoList) == 0) {
                UINT32 flag_leave = 0;

                rx_swfifo_info = udc_rxfifo_list_get_first_entry(&UdcEpInfo->RxSwFifoList);
                // Reach the request length.
                if ((total_length + rx_swfifo_info->Length) > data_size) {
                    flag_leave = 1;
                } else {
                    if (rx_swfifo_info->Length != 0U) {
                        USB_UtilityMemoryCopy(tmp_data_ptr, rx_swfifo_info->Addr, rx_swfifo_info->Length);
                    }
                    total_length += rx_swfifo_info->Length;
                    tmp_data_ptr   = &tmp_data_ptr[rx_swfifo_info->Length];

                    // del the list.
                    udc_rxfifo_list_del(&rx_swfifo_info->List);

                    // Stop when hitting short/zero-legnth packet.
                    if ((rx_swfifo_info->Length == 0U) || (rx_swfifo_info->Length < max_pkt_size)) {
                        flag_short_packet = 1;
                        flag_leave       = 1;
                    }
                }
                if (flag_leave != 0U) {
                    break;
                }
            }

            if (total_length != 0U) {
                USB_UtilityCacheInvdUInt8(*DataPtr, total_length);
            }
            *DataPtr = tmp_data_ptr;
            if (flag_short_packet != 0U) {
                *Length = 0;
            } else {
                *Length = data_size - total_length;
            }
            TransferRequest->ux_slave_transfer_request_actual_length        = total_length;
            TransferRequest->ux_slave_transfer_request_current_data_pointer = *DataPtr;
        }
    }
    rx_mutex_put();
}

/**
 * Called by driver to setup DMA descriptor for non-control transfers. DMA descripor is set as "READY".
 * */
void USBD_UdcRxDescSetup(UDC_ENDPOINT_INFO_s *UdcEpInfo, const UX_SLAVE_TRANSFER *TransferRequest, const UINT8 *DataPtr, UINT32 Length, UINT32 Update)
{
    UINT32 ux_ep_type;
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UDC_DATA_DESC_s *dma_desc;
    UINT32 physical_endpoint_index;

    usbx_endpoint    = TransferRequest->ux_slave_transfer_request_endpoint;
    ux_ep_type       = udc_get_endpoint_type(usbx_endpoint);

    dma_desc         = UdcEpInfo->DmaDesc;
    physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;

    if ((dma_desc == NULL) && (UdcEpInfo->DmaDescSize != 0U)) {
        // NULL pointer handling
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcRxDescSetup(): Tried to invalidate illegal range");
    } else {
        USB_UtilityCacheInvdDataDesc(dma_desc, UdcEpInfo->DmaDescSize);

        if (udc_is_control_endpoint_type(ux_ep_type) == 0U) {
            if (dma_desc != NULL) {
                UINT32 i;
                UINT32 data_size;
                UINT32 data_count;
                UINT32 max_pkt_size;
                UINT32 desc_number;

                max_pkt_size = udc_get_endpoint_max_packet_size(usbx_endpoint);
                data_size   = Length;
                data_count  = (data_size + max_pkt_size - 1U) / max_pkt_size;

                if (data_count == 0U) {
                    data_count = 1;
                }

                if (buffer_cache_alignment_check(DataPtr) != USB_ERR_SUCCESS) {
                    usb_error_notify(USB_ERR_BUF_NOT_CACHE_ALIGNED, USB_UtilityVoid2UInt32(DataPtr));
                }

                desc_number = UdcEpInfo->DmaDescNum;

                if (data_count > desc_number) {
                    data_count = desc_number;
                }

                for (i = 0; i < desc_number; i++) {
                    if (i < data_count) {
                        dma_desc[i].Status      = USBD_DMA_HOST_READY;
                        dma_desc[i].DataPtr     = USB_UtilityVirP8ToPhyI32(&DataPtr[i * max_pkt_size]);
                        dma_desc[i].NextDescPtr = USB_UtilityVirDataDesc2PhyI32(&dma_desc[i + 1U]);
                    } else {
                        dma_desc[i].Status      = USBD_DMA_HOST_NOT_RDY;
                        dma_desc[i].DataPtr     = 0;
                        dma_desc[i].NextDescPtr = 0;
                    }
                }
                dma_desc[data_count - 1U].NextDescPtr = 0;
                dma_desc[data_count - 1U].Status     |= USBD_DMA_LAST_DESC;

                USB_UtilityCacheFlushDataDesc(dma_desc, UdcEpInfo->DmaDescSize);

                if (Update == 1U) {
                    AmbaRTSL_UsbSetEpOutDesptr(physical_endpoint_index, USB_UtilityVirDataDesc2PhyI32(dma_desc));      // data buffer pointer
                    // Must put this at the end of this function to make sure the concurrent isr-bottomhalf task
                    // can run the correct flow according to the RxDmaState.
                    USBD_UdcRxStateMutexGet(USB_WAIT_FOREVER);
                    UdcEpInfo->RxDmaState &= ~UDC_RX_DMA_STAT_ALT_DESC;
                    UdcEpInfo->RxDmaState |= UDC_RX_DMA_STAT_REQ_PENDING;
                    USBD_UdcRxStateMutexPut();
                } else {
                    USBD_UdcRxStateMutexGet(USB_WAIT_FOREVER);
                    UdcEpInfo->RxDmaState |= UDC_RX_DMA_STAT_REQ_PENDING;
                    USBD_UdcRxStateMutexPut();
                }
            }
        } else {
            UDC_DATA_DESC_s *ctrl_rx_desc;

            // prepare control endpoint RX descriptor
            USBD_UdcCtrlOutDataDescGet(&ctrl_rx_desc);

            ctrl_rx_desc->Status = USBD_DMA_LAST_DESC;

            USB_UtilityCacheFlushDataDesc(ctrl_rx_desc, sizeof(UDC_DATA_DESC_s));
        }
    }
}
/**
 * Called by driver to setup DMA descriptor for multiple rx fix. DMA descripor is set as "READY".
 * */
void USBD_UdcRxSubDescSetup(UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    UINT32 physical_endpoint_index;
    UDC_RDMA_SUB_DESC_INFO_s *sub_dma_info;

    physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;
    sub_dma_info       = &UdcEpInfo->SubDmaInfo;

    sub_dma_info->DmaDesc.Status  = USBD_DMA_LAST_DESC;
    sub_dma_info->DmaDesc.DataPtr = USB_UtilityVirP8ToPhyI32(sub_dma_info->Buffer);

    USB_UtilityCacheFlushDataDesc(&sub_dma_info->DmaDesc, sizeof(UDC_DATA_DESC_s));

    AmbaRTSL_UsbSetEpOutDesptr(physical_endpoint_index, USB_UtilityVirDataDesc2PhyI32(&sub_dma_info->DmaDesc));      // data buffer pointer

    USBD_UdcRxStateMutexGet(USB_WAIT_FOREVER);
    UdcEpInfo->RxDmaState |= UDC_RX_DMA_STAT_ALT_DESC;
    USBD_UdcRxStateMutexPut();
}

/**
 * Called by driver to send a single NULL packet over control endpoint.
 * */
void USBD_UdcCtrlNullPacketSend(void)
{
    // Flush Tx FIFO
    AmbaRTSL_UsbSetEpInCtrlF(0, 1);

    // setup TX DMA, length is 0.
    USB_UtilityCacheInvdDataDesc(&ctrl_ep_desc.CtrlInDataDesc, sizeof(UDC_DATA_DESC_s));
    ctrl_ep_desc.CtrlInDataDesc.Status = USBD_DMA_LAST_DESC;
    USB_UtilityCacheFlushDataDesc(&ctrl_ep_desc.CtrlInDataDesc, sizeof(UDC_DATA_DESC_s));

    // Clear Nak should send the zero-length data
    if (AmbaRTSL_UsbGetEpInNAK(0) != 0U) {
        AmbaRTSL_UsbClearEpInNAK(0, 1);
    }
}
/**
 * Called by driver to send an extra NULL packet over control endpoint if the transfer size is a multiple of endpoint maximum packet size.
 * */
void USBD_UdcCtrlNullPacketSendEx(void)
{
    // disable USB interrupt
    USBD_IntDisable();

    // NAK CTRL-OUT and disable RX DMA. This is to avoid next CTRL-OUT-ACK occurance.
    AmbaRTSL_UsbSetEpOutNAK(0, 1);
    AmbaRTSL_UsbSetDevCtlRde(0);

    // Flush Tx FIFO
    AmbaRTSL_UsbSetEpInCtrlF(0, 1);

    // Clear CTRL-IN NAK
    if (AmbaRTSL_UsbGetEpInNAK(0) != 0U) {
        AmbaRTSL_UsbClearEpInNAK(0, 1);
    }

    // setup TX DMA, length is 0.
    USB_UtilityCacheInvdDataDesc(&ctrl_ep_desc.CtrlInDataDesc, sizeof(UDC_DATA_DESC_s));
    ctrl_ep_desc.CtrlInDataDesc.Status = USBD_DMA_LAST_DESC;

    USB_UtilityCacheFlushDataDesc(&ctrl_ep_desc.CtrlInDataDesc, sizeof(UDC_DATA_DESC_s));

    AmbaRTSL_UsbSetEpInPollDemand(0, 1);

    // assume TX DMA is done.

    // enable USB interrupt
    USBD_IntEnable();
}
/**
 * Called by driver to setup DMA descriptor for control-in transfer.
 * */
void USBD_UdcTxCtrlDescSetup(UX_SLAVE_TRANSFER *Transfer)
{
    UINT32 data_length;
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UDC_DATA_DESC_s *desc = &ctrl_ep_desc.CtrlInDataDesc;
    UINT8 *data_ptr;

    // Get the content of the DMA buffer into the client buffer.
    data_ptr = USB_UtilityPhyI32ToVirU8P(desc->DataPtr);

    usbx_endpoint  = Transfer->ux_slave_transfer_request_endpoint;
    data_length = Transfer->ux_slave_transfer_request_in_transfer_length;

    if (data_length > usbx_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize) {
        data_length = usbx_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
    }

    USB_UtilityMemoryCopy(data_ptr, Transfer->ux_slave_transfer_request_current_data_pointer, data_length);

    // some AmbaRTSL_CacheFlushDataCacheRange() has a bug that it can't aceept size = 1
    // To avoid this happening again and again, use size=16 instead.
    if (data_length == 1U) {
        USB_UtilityCacheFlushUInt8(data_ptr, 16);
    } else {
        USB_UtilityCacheFlushUInt8(data_ptr, data_length);
    }

    desc->Status = (USBD_DMA_LAST_DESC | data_length);

    USB_UtilityCacheFlushDataDesc(&ctrl_ep_desc.CtrlInDataDesc, sizeof(UDC_DATA_DESC_s));

    Transfer->ux_slave_transfer_request_current_data_pointer = &Transfer->ux_slave_transfer_request_current_data_pointer[data_length];
    Transfer->ux_slave_transfer_request_actual_length       += data_length;
    Transfer->ux_slave_transfer_request_in_transfer_length  -= data_length;
}

static UINT32 txdesc_setup_loop_count_get(UINT32 LoopCount, UINT32 MaxChainNumber)
{
    UINT32 uret = LoopCount;
    if (uret == 0U) {
        uret = 1;
    }

    if (uret > MaxChainNumber) {
        uret = MaxChainNumber;
    }
    return uret;
}

/**
 * Called by driver to setup DMA descriptor for non-control-in transfer.
 * */
void USBD_UdcTxDescSetup(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *Transfer, UINT32 FlagIsoMode)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UDC_DATA_DESC_s *dma_desc = NULL;
    UINT32 max_chain_number = 0;
    UINT32 i;
    const UINT8 *ptr = Transfer->ux_slave_transfer_request_current_data_pointer;
    UINT32 flag_done      = 0;
    UDC_ISO_DESC_INFO_s *iso_desc_info = NULL;

    usbx_endpoint = Transfer->ux_slave_transfer_request_endpoint;

    if (FlagIsoMode != 0U) {
        UINT32 WrapperIndex;
        // Check if there is an available descriptor wrapper
        for (WrapperIndex = 0; WrapperIndex < USBD_UDC_ISO_WRAPPER_NUM; WrapperIndex++) {
            iso_desc_info = &UdcEpInfo->IsoDescPool[WrapperIndex];

            if (iso_desc_info->Used == 0U) {
                iso_desc_info->Used = 1;
                dma_desc            = iso_desc_info->DataDesc;
                break;
            }
        }

        if (WrapperIndex == USBD_UDC_ISO_WRAPPER_NUM) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcTxDescSetup: no available descriptor wrapper");
            flag_done = 1;
        } else {
            max_chain_number = UdcEpInfo->DmaDescNum;
        }
    } else {
        dma_desc        = UdcEpInfo->DmaDesc;
        max_chain_number = UdcEpInfo->DmaDescNum;

        if ((dma_desc == NULL) && (UdcEpInfo->DmaDescSize != 0U)) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcTxDescSetup: Tried to access illegal range");
            flag_done = 1;
        } else {
            USB_UtilityCacheInvdDataDesc(dma_desc, UdcEpInfo->DmaDescSize);
        }
    }

    if (flag_done == 0U) {
        if (dma_desc != NULL) {
            UINT32 data_size;
            UINT32 loop_count;
            UINT32 max_pkt_size;
            UINT32 transmit_size;

            max_pkt_size = udc_get_endpoint_max_packet_size(usbx_endpoint);
            data_size   = Transfer->ux_slave_transfer_request_in_transfer_length;
            loop_count  = (data_size + max_pkt_size - 1U) / max_pkt_size;

            if (buffer_cache_alignment_check(ptr) != USB_ERR_SUCCESS) {
                usb_error_notify(USB_ERR_BUF_NOT_CACHE_ALIGNED, USB_UtilityVoid2UInt32(ptr));
            }

            USB_UtilityCacheFlushUInt8(ptr, data_size);

            loop_count = txdesc_setup_loop_count_get(loop_count, max_chain_number);

            for (i = 0; i < loop_count; i++) {
                if (Transfer->ux_slave_transfer_request_in_transfer_length > max_pkt_size) {
                    transmit_size = max_pkt_size;
                } else {
                    transmit_size = Transfer->ux_slave_transfer_request_in_transfer_length;
                }

                dma_desc[i].Status  = transmit_size;
                dma_desc[i].DataPtr = USB_UtilityVirP8ToPhyI32(Transfer->ux_slave_transfer_request_current_data_pointer);

                Transfer->ux_slave_transfer_request_current_data_pointer = &Transfer->ux_slave_transfer_request_current_data_pointer[transmit_size];
                Transfer->ux_slave_transfer_request_actual_length       += transmit_size;
                Transfer->ux_slave_transfer_request_in_transfer_length  -= transmit_size;
            }

            dma_desc[loop_count - 1U].Status |= USBD_DMA_LAST_DESC;

            if (FlagIsoMode != 0U) {
                // Push descriptor wrapper to the queue
                UDC_ISO_REQUEST_s IsoReq;
                IsoReq.DescWrapper     = iso_desc_info;
                IsoReq.TransferRequest = Transfer;
                IsoReq.StartTimeStamp  = tx_time_get();
                IsoReq.EndTimeStamp    = IsoReq.StartTimeStamp + Transfer->ux_slave_transfer_request_timeout;

                if (USB_UtilityQueueIsoSend(&UdcEpInfo->IsoReqQueue, &IsoReq, USB_NO_WAIT) != USB_ERR_SUCCESS) {
                    // can't send queue
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcTxDescSetup: can't send Iso request");
                }
            }

            USB_UtilityCacheFlushDataDesc(dma_desc, loop_count * sizeof(UDC_DATA_DESC_s));
        }
    }
}

/**
 * Called by driver to setup DMA descriptor for Zero-Length-Packet IN transfer.
 * */
void USBD_UdcTxDescSetupZeroLength(const UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    static UINT8 udc_dummy_buf[1] __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE)));    // buffer for Zero Length packet
    UDC_DATA_DESC_s *desc = UdcEpInfo->DmaDesc;

    if (UdcEpInfo->DmaDesc != NULL) {

        udc_dummy_buf[0] = 0;
        USB_UtilityCacheInvdDataDesc(UdcEpInfo->DmaDesc, sizeof(UDC_DATA_DESC_s));

        desc[0].Status  = USBD_DMA_LAST_DESC;
        desc[0].DataPtr = USB_UtilityVirP8ToPhyI32(udc_dummy_buf);

        USB_UtilityCacheFlushDataDesc(desc, sizeof(UDC_DATA_DESC_s));
    } else {
        // NULL pointer handling
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcTxDescSetupZeroLength: Tried to access illegal range");
    }
}

/**
 * Called by driver to empty both TX and RX FIFO.
 * */
void USBD_UdcDmaFifoEmpty(void)
{
    UINT32 endpoint_index;
    UINT32 max_flush_count = ((UDC_MAX_ENDPOINT_NUM >> 1U) + 1U);
    INT32  retry_count;

    // Empty TX FIFO
    for (endpoint_index = 0; endpoint_index < max_flush_count; endpoint_index++) {
        AmbaRTSL_UsbSetEpInCtrlF(endpoint_index, 1);
    }

    // Empty RX FIFO
    if (AmbaRTSL_UsbGetDevStatusRfEmpty() != 0U) {
        ;
        // Switch to slave mode
        AmbaRTSL_UsbSetDevCtlMode(0);
        // Read RX_FIFO until it's empty
        for (retry_count = 10000; retry_count > 0; retry_count--) {
            if (AmbaRTSL_UsbGetDevStatusRfEmpty() != 0U) {
                break;
            } else {
                UINT32 dummy;
                // Read FIFO and delay for FIFO_EMPTY to change
                dummy = AmbaRTSL_USBReadAddress(0x0800U);
                for (dummy = 0; dummy < 1000U; dummy++) {
                    ;
                }
            }
        }

        if (retry_count <= 0) {
            // error handling
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "[USBD] AmbaUSB_Drv_EmptyDmaFifo: failed");
        }

        AmbaRTSL_UsbSetDevCtlMode(USB_RTSL_DEV_DMA_MODE);
    }
}

/**
 * Called by driver to initialize USB Device Controller.
 * */
void USBD_UdcInitController(void)
{
    UINT32 i;
    UINT32 max_ep_num = ((UDC_MAX_ENDPOINT_NUM >> 1U) + 1U);

    // DO NOT reset controller here. It might cause D+ glitch.
    // Instead, controller reset would be done during VBUS disconnect.

    // setup Device controller
    // initialize dev_config register
    AmbaRTSL_UsbSetDevConfig(0);                                    // initial clear
    // start to set each bit
    if (usbd_sys_config_udc_local != NULL) {
        if (usbd_sys_config_udc_local->InitSpeed == USB_CONNECT_SPEED_HIGH) {
            AmbaRTSL_UsbSetDevConfigSpd(0);
        } else {
            AmbaRTSL_UsbSetDevConfigSpd(1);
        }
    } else {
        AmbaRTSL_UsbSetDevConfigSpd(0);
    }
    AmbaRTSL_UsbSetDevConfigRwkp(USB_RTSL_DEV_REMOTE_WAKEUP);       // remote wakeup
    AmbaRTSL_UsbSetDevConfigSp(USB_RTSL_DEV_SELF_POWER);            // self-power
    AmbaRTSL_UsbSetDevConfigPyType(USB_RTSL_DEV_PHY_8BIT);
    AmbaRTSL_UsbSetDevConfigDynProg(USB_RTSL_DEV_CSR_PRG);
    AmbaRTSL_UsbSetDevConfigRev();
    AmbaRTSL_UsbSetDevConfigHaltSts(USB_RTSL_DEV_HALT_ACK);
    AmbaRTSL_UsbSetDevConfigSetDesc(USB_RTSL_DEV_SET_DESC_STALL);

    // initialize dev_control register
    AmbaRTSL_UsbSetDevCtl(0x400);                                   // soft disconnect during initialization
    AmbaRTSL_UsbSetDevCtlBe(USB_RTSL_DEV_LITTLE_ENDN);              // little endian
    AmbaRTSL_UsbSetDevCtlBren(1);
    AmbaRTSL_UsbSetDevCtlBrlen(15);
    AmbaRTSL_UsbSetDevCtlMode(USB_RTSL_DEV_DMA_MODE);
    AmbaRTSL_UsbSetDevCtlTde(1);                                    // enable TX DMA
    AmbaRTSL_UsbSetDevCtlRde(1);                                    // enable RX DMA

    AmbaRTSL_UsbEnInEpInt(0);
    AmbaRTSL_UsbDisInEpInt(1);
    AmbaRTSL_UsbDisInEpInt(2);
    AmbaRTSL_UsbDisInEpInt(3);
    AmbaRTSL_UsbDisInEpInt(4);
    AmbaRTSL_UsbDisInEpInt(5);

    AmbaRTSL_UsbEnOutEpInt(0);
    AmbaRTSL_UsbDisOutEpInt(1);
    AmbaRTSL_UsbDisOutEpInt(2);
    AmbaRTSL_UsbDisOutEpInt(3);
    AmbaRTSL_UsbDisOutEpInt(4);
    AmbaRTSL_UsbDisOutEpInt(5);

    // initialize DEV int
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_ALL, 0);                  // unmask all firstly and then mask each one
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_SOF, 1);
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_RMTWKP, 1);

    AmbaRTSL_UsbSetEpInCtrlF(0, 1);                                 // flush TX FIFO to prepare descriptor

    // reset all endpoints
    for (i = 0; i < max_ep_num; i++) {
        AmbaRTSL_UsbSetEpOutCtrl(i, 0);
        AmbaRTSL_UsbSetEpOutMaxpksz(i, 0);
        AmbaRTSL_UsbSetEpOutStatus(i, 0);

        AmbaRTSL_UsbSetEpInCtrl(i, 0);
        AmbaRTSL_UsbSetEpInMaxpksz(i, 0);
        AmbaRTSL_UsbSetEpInStatus(i, 0);
    }

    // initialize EP0
    AmbaRTSL_UsbSetEpOutType(0, 0);                                                                 // control type endpoint
    AmbaRTSL_UsbSetEpOutRxReady(0, 1);                                                                      // receive ready
    AmbaRTSL_UsbSetEpOutMaxpksz(0, 64);
    AmbaRTSL_UsbSetEpOutSetupbuf(0, USB_UtilityVirSetupDesc2PhyI32(&ctrl_ep_desc.CtrlOutSetupDesc)); // setup buffer pointer
    AmbaRTSL_UsbSetEpOutDesptr(0, USB_UtilityVirDataDesc2PhyI32(&ctrl_ep_desc.CtrlOutDataDesc));     // data buffer pointer

    AmbaRTSL_UsbSetEpInBuffsize(0, (64 / 4));                                                               // unit = 32bits
    AmbaRTSL_UsbSetEpInMaxpksz(0, 64);
    AmbaRTSL_UsbSetEpInDesptr(0, USB_UtilityVirDataDesc2PhyI32(&ctrl_ep_desc.CtrlInDataDesc));


    // init UDC20 controller
    // init ep 0
    AmbaRTSL_UsbSetEp20(0, 0);                                      // reset
    AmbaRTSL_UsbSetEp20EndPointType(0, UX_CONTROL_ENDPOINT);        // control type
    AmbaRTSL_UsbSetEp20EndPointDir(0, 1);                           // FIXME: IN direction or OUT, no matter for control endpoint #0 ?
    AmbaRTSL_UsbSetEp20LogicalID(0, 0);
    AmbaRTSL_UsbSetEp20MaxPktSize(0, 64);
    AmbaRTSL_UsbSetEp20Interface(0, 0);
    AmbaRTSL_UsbSetEp20AltSetting(0, 0);
}

/**
 * Called by driver to initialize USB Device Driver/Controller.
 * */
UINT32 USBD_UdcInit(void)
{
    UINT32 uret;

    uret = USBD_SystemConfigGet(&usbd_sys_config_udc_local);
    if (uret == USB_ERR_SUCCESS) {
        // Initialize the UDC parameter.
        parameters_init();
        // create resource.
        uret = resource_create();
        if (uret == USB_ERR_SUCCESS) {
            // Phy Setup
            phy_setup();
            // Init the control-setup descriptor.
            USBD_UdcInitCtrlSetupDesc();
            // Init the control-data descriptor.
            USBD_UdcInitCtrlDataDesc();
            // Flush the TX/RX FIFO.
            USBD_UdcDmaFifoEmpty();
            // Init the device controller.
            USBD_UdcInitController();
            // Init the device driver.
            driver_init();
            // Init the control endpoint.
            control_endpoint_create();
            // Init the Setup handler.
            uret = USBD_SetupInit();
        }

        // Release resouce if init fail.
        if (uret != USB_ERR_SUCCESS) {
            (void)USBD_UdcDeInit();
        }
    }

    return uret;
}

/**
 * Called by driver to de-initialize USB Device Driver/Controller.
 * */
UINT32 USBD_UdcDeInit(void)
{
    UINT32 uret;
    // Flush the TX/RX FIFO.
    USBD_UdcDmaFifoEmpty();
    // deinit the Setup handler.
    uret = USBD_SetupDeInit();
    // deinit the control endpoint.
    control_endpoint_destroy();
    // deinit the driver.
    driver_deinit();
    return uret;
}

/**
 * Called by driver to pull up/down D+ line.
 * */
void USBD_UdcDeviceDataConnSet(UINT32 value)
{
    if (value == 0U) {
        UINT32 power_off = 0;
        // Check USB power status.
        if (AmbaRTSL_RctGetUsbPort0State() != USB_ON) {
            USB_PhyPllEnable(1, UPORT_MODE_DEVICE);
            power_off = 1;
        }
        AmbaRTSL_UsbSetDevCtlSd(1);

        // Restore the original power state.
        if (power_off != 0U) {
            USB_PhyPllEnable(0, UPORT_MODE_DEVICE);
        }
    } else {
        if (AmbaRTSL_RctGetUsbPort0State() != USB_ON) {
            USB_PhyPllEnable(1, UPORT_MODE_DEVICE);
        }
        AmbaRTSL_UsbSetDevCtlSd(0);
        USBD_ConnectChgCfgCntStartSet(1);
    }
}

/**
 * Called by driver to get UDC instance
 * */
UINT32 USBD_UdcUdcInstanceGet(USBD_UDC_s **Udc)
{
    UINT32 uret;

    if ((flag_udc_init & USBD_FLAG_USBD_DRIVER_INIT) != 0U) {
        *Udc = &local_usbd_udc;
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_NO_INIT;
    }
    return uret;
}

/**
 * Called by driver to get endpoint map information by logical endpoint address.
 * */
UDC_ENDPOINT_MAP_s *USBD_UdcEndpointMapGet(USBD_UDC_s *Udc, UINT32 LogAddr)
{
    static UDC_ENDPOINT_MAP_s ep0_map = {
        0, // Logical Endpoint address
        0, // UDC20 Endpoint Index
        0, // Physical Endpoint address
        0, // Endpoint Direction
        1  // Used
    };

    UDC_ENDPOINT_MAP_INFO_s *map_info = &Udc->EndpointMapInfo;
    UINT32 i;
    UDC_ENDPOINT_MAP_s *ret_map = NULL;

    if (LogAddr == 0U) {
        // control endpoint would be 0, and the map might be empty in some flow.
        // make the map for endpoint 0 always available.
        ret_map = &ep0_map;
    } else {
        for (i = 0; i < UDC_MAX_ENDPOINT_NUM; i++) {
            UDC_ENDPOINT_MAP_s *map = &(map_info->EndpointMap[i]);
            if ((map->LogicalAddr == LogAddr) && (map->Used != 0U)) {
                ret_map = map;
                break;
            }
        }
    }

    if (ret_map == NULL) {
        // error handling
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_UdcEndpointMapGet(): No map for logical address 0x%X.", LogAddr, 0, 0, 0, 0);
    }

    return ret_map;
}

/**
 * Called by driver to get enumerated USB speed.
 * */
UINT32 USBD_UdcEnumSpeedGet(void)
{
    UINT32 speed;

    if (AmbaRTSL_UsbGetDevStatusEnumSpd() == (UINT32)USB_RTSL_DEV_SPD_HI) {
        speed = USB_CONNECT_SPEED_HIGH;
    } else {
        speed = USB_CONNECT_SPEED_FULL;
    }
    return speed;
}

/**
 * Called by driver to get if USB device is configured by Host.
 * */
UINT32 USBD_UdcIsConfigured(void)
{
    UINT32 uret;

    if ((flag_udc_init & USBD_FLAG_USBD_DRIVER_INIT) != 0U) {
        if (AmbaRTSL_UsbGetDevStatusCfg() != 0U) {
            uret = 1;
        } else {
            uret = 0;
        }
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_UdcIsConfigured(): The configure index is meaningless when hw is not init");
        uret = 0;
    }

    return uret;
}
/** @} */
