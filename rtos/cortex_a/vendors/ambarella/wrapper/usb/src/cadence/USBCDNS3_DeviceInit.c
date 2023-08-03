/**
 *  @file USBCDNS3_DeviceInit.c
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
 *  @details USB kernel driver to complete hw initialization.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

#define USBD_FLAG_USBD_DRIVER_INIT         ((UINT32)0x00000008UL)
#define USBD_FLAG_USBD_INT_QUEUE_INIT      ((UINT32)0x00000020UL)

static UINT32             flag_udc_init              = 0;

static UINT32 resource_isr_queue_create(void)
{
    static UINT8  isr_queue_memory[USBD_ISR_QUEUE_MEM_SIZE] __attribute__((section(".bss.noinit")));
    USBD_UDC_s   *udc = USBCDNS3_UdcInstanceGet();
    UINT32        uret = USB_ERR_SUCCESS;

    if ((flag_udc_init & USBD_FLAG_USBD_INT_QUEUE_INIT) == 0U) {
        USB_UtilityMemorySet(isr_queue_memory, 0, sizeof(isr_queue_memory));
        uret = USB_UtilityQueueCreate(&udc->IsrRequestQueue,
                                      isr_queue_memory,
                                      (UINT32)sizeof(UDC_ISR_REQUEST_s),
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
    UINT32 uret;

    // Create ISR queue.
    uret = resource_isr_queue_create();

    return uret;
}

static void ux_device_driver_init(void)
{
    UX_SLAVE_DCD *ux_dcd;
    USBD_UDC_s   *udc = USBCDNS3_UdcInstanceGet();
    void         *des_ptr;
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();

    if ((flag_udc_init & USBD_FLAG_USBD_DRIVER_INIT) == 0U) {
        // Get the pointer to the USBX DCD.
        ux_dcd = &_ux_system_slave->ux_system_slave_dcd;

        // The controller initialized here is of UDC2.
        ux_dcd->ux_slave_dcd_controller_type = USBD_SLAVE_CONTROLLER_TYPE_UDC3;

        // Set the pointer to the DCD.
        USB_UtilityMemoryCopy(&des_ptr, &udc, sizeof(void *));
        ux_dcd->ux_slave_dcd_controller_hardware = des_ptr;

        // Save the base address of the controller.
        ux_dcd->ux_slave_dcd_io = (ULONG)hws->Udc32.BaseAddress;

        // Set the generic DCD owner.
        udc->UxDcdOwner = ux_dcd;

        // Initialize the function collector for this DCD.
        ux_dcd->ux_slave_dcd_function = USBD_DrvDispatchFunction;

        // Set the state of the controller to OPERATIONAL now.
        ux_dcd->ux_slave_dcd_status = UX_DCD_STATUS_OPERATIONAL;

        flag_udc_init |= USBD_FLAG_USBD_DRIVER_INIT;
    }
}

static void ux_device_driver_deinit(void)
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


static void control_endpoint_create(void)
{
    UX_SLAVE_DEVICE     *usbx_device   = &_ux_system_slave->ux_system_slave_device;
    UDC_ENDPOINT_INFO_s *udc_ep_info   = USBDCDNS3_EndpointInfoGet(XHCI_EP0_CONTEXT_OFFSET);
    UX_SLAVE_ENDPOINT   *usbx_endpoint = &usbx_device->ux_slave_device_control_endpoint;

    // Get the pointer to the device.
    usbx_device->ux_slave_device_control_endpoint.ux_slave_endpoint_descriptor.bEndpointAddress = 0;
    usbx_device->ux_slave_device_control_endpoint.ux_slave_endpoint_descriptor.bmAttributes     = 0;
    usbx_device->ux_slave_device_control_endpoint.ux_slave_endpoint_descriptor.wMaxPacketSize   = 64;

    // Hook control endpoint.
    udc_ep_info->UxEndpoint             = usbx_endpoint;
    udc_ep_info->PhysicalEpIndex        = XHCI_EP0_CONTEXT_OFFSET;
    usbx_endpoint->ux_slave_endpoint_ed = USB_UtilityUdcEd2VoidP(udc_ep_info);

    USBD_UdcEndpointBufferAllocate(&usbx_device->ux_slave_device_control_endpoint);

}


static void control_endpoint_destroy(void)
{
    UX_SLAVE_DEVICE     *usbx_device   = &_ux_system_slave->ux_system_slave_device;
    UDC_ENDPOINT_INFO_s *udc_ep_info   = USBDCDNS3_EndpointInfoGet(XHCI_EP0_CONTEXT_OFFSET);
    UX_SLAVE_ENDPOINT   *usbx_endpoint = &usbx_device->ux_slave_device_control_endpoint;

    // Get the pointer to the device.
    udc_ep_info->UxEndpoint             = NULL;
    usbx_endpoint->ux_slave_endpoint_ed = NULL;
    USBD_UdcEndpointBufferDestroy(&usbx_device->ux_slave_device_control_endpoint);
}


/** \addtogroup Internal_APIs
 *  @{
 * */

/**
 * Called by driver to initialize USB Device Driver/Controller.
 */
UINT32 USBDCDNS3_ControllerInit(const USBDCDNS3_CALLBACKS_s *CallBacks)
{
    UINT32 uret;
    static USBD_SYS_CONFIG_s *usbd_sys_config_udc_local  = NULL;

    uret = USBD_SystemConfigGet(&usbd_sys_config_udc_local);
    if (uret == USB_ERR_SUCCESS) {

        // Phy Setup
        USBCDNS3_PHYUp();

        // Initialize the UDC parameter.
        USBCDNS3_XhcParameterInit(USBCDNS3_MODE_DEVICE);

        // create resource.
        uret = resource_create();

        if (uret == USB_ERR_SUCCESS) {

            // set event callbacks for disconnect/connect/setup/...
            USBDCDNS3_EventCallbackSet(CallBacks);

            // Init the device controller.
            uret = USBCDNS3_XhcInitRun(USBCDNS3_CTRLER_MODE_DEVICE);

            if (uret == 0U) {
                // Init the device driver.
                ux_device_driver_init();

                // Init the control endpoint.
                control_endpoint_create();

                // Init the Setup handler.
                uret = USBD_SetupInit();
            }
        }

        // Release resouce if init fail.
        if (uret != USB_ERR_SUCCESS) {
            //(void)USBD_UdcDeInit();
        }
    }
    return uret;
}

UINT32 USBDCDNS3_ControllerDeInit(void)
{
    XHC_CDN_DRV_RES_s           *drv_res  = USBCDNS3_DrvInstanceGet();

    (void)USBCDNS3_SlotDisable(drv_res);

    // wait 500 ms to let ISR task have time to check events.
    USB_UtilityTaskSleep(500);

    (void)USBCDNS3_XhcStop();

    // turn down Controller and PHY
    USBCDNS3_ControllerDown();
    USBCDNS3_PHYDown();

    // delete semaphore
    {
        if (USB_UtilitySemaphoreDelete(&drv_res->SemaphoreCommandQueue) != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "can't delete semaphore for command queue");
        }
        if (USB_UtilitySemaphoreDelete(&drv_res->SemaphoreEp0Transfer) != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "can't delete semaphore for EP0 queue");
        }
    }

    control_endpoint_destroy();
    ux_device_driver_deinit();
    return 0;
}


/**
 * Called by USB driver to complete initialization
 * */
UINT32 USBDCDNS3_DrvInitComplete(void)
{
    const UX_SLAVE_DCD      *ux_dcd;
    const UX_SLAVE_ENDPOINT *source_ptr;
    const void              *destination_ptr;
    UX_SLAVE_DEVICE         *ux_device;
    UX_SLAVE_TRANSFER       *transfer_request;
    UINT8                   *device_framework;
    UINT32                   uret;

    // Get the pointer to the DCD.
    ux_dcd = &_ux_system_slave->ux_system_slave_dcd;

    // Get the pointer to the device.
    ux_device = &_ux_system_slave->ux_system_slave_device;

    // The device is operating at full/high/ss/ssp speed, default is high speed
    // reply descriptor to host according to speed enumeration result

    if ((_ux_system_slave->ux_system_slave_speed == (UINT32)UX_SUPER_SPEED_DEVICE) ||
        (_ux_system_slave->ux_system_slave_speed == (UINT32)UX_SUPER_SPEED_PLUS_DEVICE)) {
        _ux_system_slave->device_framework        = _ux_system_slave->device_framework_super_speed;
        _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_super_speed;
    } else if (_ux_system_slave->ux_system_slave_speed == (UINT32)UX_HIGH_SPEED_DEVICE) {
        _ux_system_slave->device_framework        = _ux_system_slave->device_framework_high_speed;
        _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_high_speed;
    } else {
        _ux_system_slave->device_framework        = _ux_system_slave->device_framework_full_speed;
        _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_full_speed;
    }

    // Get the device framework pointer.
    device_framework = _ux_system_slave->device_framework;

    // And create the decompressed device descriptor structure.
    _ux_utility_descriptor_parse(device_framework,
                                 _ux_system_device_descriptor_structure,
                                 UX_DEVICE_DESCRIPTOR_ENTRIES,
                                 (UINT8 *) &ux_device->ux_slave_device_descriptor);

    // Now we create a transfer request to accept the first SETUP packet
    // and get the ball running. First get the address of the endpoint
    // transfer request container.
    transfer_request = &ux_device->ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

    // Set the timeout to be for Control Endpoint.
    transfer_request->ux_slave_transfer_request_timeout = UX_CONTROL_TRANSFER_TIMEOUT;

    // Adjust the current data pointer as well.
    transfer_request->ux_slave_transfer_request_current_data_pointer =
        transfer_request->ux_slave_transfer_request_data_pointer;

    // Update the transfer request endpoint pointer with the default endpoint.
    transfer_request->ux_slave_transfer_request_endpoint = &ux_device->ux_slave_device_control_endpoint;

    // The control endpoint max packet size needs to be filled manually in its descriptor.
    transfer_request->ux_slave_transfer_request_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize =
        ux_device->ux_slave_device_descriptor.bMaxPacketSize0;

    // On the control endpoint, always expect the maximum.
    transfer_request->ux_slave_transfer_request_requested_length =
        ux_device->ux_slave_device_descriptor.bMaxPacketSize0;

    // Attach the control endpoint to the transfer request.
    transfer_request->ux_slave_transfer_request_endpoint = &ux_device->ux_slave_device_control_endpoint;

    // Create the default control endpoint attached to the device.
    // Once this endpoint is enabled, the host can then send a setup packet
    // The device controller will receive it and will call the setup function
    // module.
    source_ptr = &ux_device->ux_slave_device_control_endpoint;
    USB_UtilityMemoryCopy(&destination_ptr, &source_ptr, sizeof(void*));
    uret = ux_dcd->ux_slave_dcd_function(ux_dcd, UX_DCD_CREATE_ENDPOINT, destination_ptr);

    if (uret == 0U) {
        // Ensure the control endpoint is properly reset.
        ux_device->ux_slave_device_control_endpoint.ux_slave_endpoint_state = UX_ENDPOINT_RESET;

        // A SETUP packet is a DATA IN operation.
        transfer_request->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_IN;

        // Check the status change callback.
        if (_ux_system_slave->ux_system_slave_change_function != UX_NULL) {
            // Inform the application if a callback function was programmed.
            // (todo) shall we handle the return value?
            if (_ux_system_slave->ux_system_slave_change_function(UX_DEVICE_ATTACHED) != 0U) {
                // action TBD
            }
        }
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }

    // We are now ready for the USB device to accept the first packet when connected.
    return uret;
}

UINT32 USBDCDNS3_UdcInstanceSafeGet(struct USBD_UDC_ts **Udc)
{
    UINT32 uret;

    if ((flag_udc_init & USBD_FLAG_USBD_DRIVER_INIT) != 0U) {
        *Udc = USBCDNS3_UdcInstanceGet();
        uret = USB_ERR_SUCCESS;
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBDCDNS3_UdcInstanceSafeGet(): no init.");
        uret = USB_ERR_NO_INIT;
    }
    return uret;
}


/** @} */
