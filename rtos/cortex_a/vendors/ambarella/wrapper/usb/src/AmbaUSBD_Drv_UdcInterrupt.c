/**
 *  @file AmbaUSBD_Drv_UdcInterrupt.c
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
 *  @details USB kernel driver interrupt handler.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <usbx/ux_device_stack.h>

#define USBD_FLAG_USBD_INT_TASK_INIT 0x00000001UL

typedef struct {
    UINT32 SetConfig :          1;      // [0] SetConfiguration command is received (WC)
    UINT32 SetInterface :       1;      // [1] SetInterface command is received (WC)
    UINT32 Empty :              1;      // [2] USB bus idle state has been detected for 3ms (WC)
    UINT32 UsbReset :           1;      // [3] A reset is detected on the USB (WC)
    UINT32 UsbSuspend :         1;      // [4] A suspend is detected on the USB (WC)
    UINT32 Sof :                1;      // [5] A SOF token is detected on the USB (WC)
    UINT32 EnumDone :           1;      // [6] Speed enumeration is completed (WC)
    UINT32 RemoteWakeUp :       1;      // [7] A Set/Clear Feature (Remote Wakeup) is received (WC)
    UINT32 Reserved :           24;     // [31:8]
} USBD_DEV_INT_STATUS_s;

static UINT32 flag_int_init           = 0;
static USBD_SYS_CONFIG_s *usbd_sys_config_int_local = NULL;

static UINT32 udc_isr_deinit(void);

static void isr_control_semaphore_reset(void)
{
    UX_SLAVE_DEVICE *usbx_device;
    UX_SLAVE_TRANSFER *transfer_request;
    UINT32 semaphore_count;

    // Get the pointer to the device.
    usbx_device      = &_ux_system_slave->ux_system_slave_device;
    transfer_request = &usbx_device->ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;
    if (USB_UtilitySemaphoreQuery(&transfer_request->ux_slave_transfer_request_semaphore, &semaphore_count) == USB_ERR_SUCCESS) {
        if (semaphore_count != 0U) {
            if (USB_UtilitySemaphoreGive(&transfer_request->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
                // shall not be here.
            }
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "Reset & Release Semaphore");
        }
    }
}

static void isr_usb_reset_notify(void)
{
    if ((usbd_sys_config_int_local != NULL) &&
        (usbd_sys_config_int_local->SystemEventCallback != NULL)) {
        if (usbd_sys_config_int_local->SystemEventCallback(USBD_SYSTEM_EVENT_RESET, 0, 0, 0) != 0U) {
            // ignore error return
        }
    }
    // Reset semaphore of control ep if necessary.
    isr_control_semaphore_reset();
}

static void isr_set_interface_notify(UINT32 InterfaceIndex, UINT32 AlternativeIndex)
{
    if ((usbd_sys_config_int_local != NULL) &&
        (usbd_sys_config_int_local->SystemEventCallback != NULL)) {
        if (usbd_sys_config_int_local->SystemEventCallback(USBD_SYSTEM_EVENT_SET_INTERFACE, InterfaceIndex, AlternativeIndex, 0) != 0U) {
            // ignore error return
        }
    }
}
static void isr_set_configure_notify(const UINT32 ConfigIndex)
{
    if ((usbd_sys_config_int_local != NULL) &&
        (usbd_sys_config_int_local->SystemEventCallback != NULL)) {
        if (usbd_sys_config_int_local->SystemEventCallback(USBD_SYSTEM_EVENT_CONFIGURED, ConfigIndex, 0, 0) != 0U) {
            // ignore error return
        }
    }
}

static void isr_usb_resume_notify(void)
{
    if ((usbd_sys_config_int_local != NULL) &&
        (usbd_sys_config_int_local->SystemEventCallback != NULL)) {
        if (usbd_sys_config_int_local->SystemEventCallback(USBD_SYSTEM_EVENT_RESUMED, 0, 0, 0) != 0U) {
            // ignore error return
        }
    }
}

static void isr_usb_suspend_notify(void)
{
    if ((usbd_sys_config_int_local != NULL) &&
        (usbd_sys_config_int_local->SystemEventCallback != NULL)) {
        if (usbd_sys_config_int_local->SystemEventCallback(USBD_SYSTEM_EVENT_SUSPENDED, 0, 0, 0) != 0U) {
            // ignore error return
        }
    }
}

static void isr_handler_reset(const USBD_UDC_s *Udc)
{
    const UDC_ENDPOINT_INFO_s *udc_ep_info;

    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_UR); // ack to clear

    // set default speed per the value set by application
    switch (AmbaRTSL_UsbGetDevStatusEnumSpd()) {
    case USB_RTSL_DEV_SPD_HI:
        _ux_system_slave->ux_system_slave_speed = UX_HIGH_SPEED_DEVICE;
        break;

    case USB_RTSL_DEV_SPD_FU:
        _ux_system_slave->ux_system_slave_speed = UX_FULL_SPEED_DEVICE;
        break;

    default:
        _ux_system_slave->ux_system_slave_speed = UX_FULL_SPEED_DEVICE;
        break;
    }

    // Check if the endpoint was already enabled. This test is necessary
    // because this UDC sometimes issues 2 bus reset. If a reset is received
    // we do not awake the stack, but rearm the default endpoint.

    udc_ep_info = &Udc->DcdEndpointsInfoArray[0];
    if ((udc_ep_info->EpState & USBD_UDC_ED_STATUS_USED) != 0U) {
        // DONOT remove this unless no plug-in/out issue
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "[USBD] USB Reset by Host");
        if (AmbaRTSL_UsbVbusVicRawStaus() == 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "[USBD] USB VBUS disconnected when reset");
            // USB reset due to USB cable removed.
        } else {
            // To Pass USBCV20:
            // USB reset due to Host Reset.
            // We should clear all endpoints including Control Endpoint.
            // This is because after Reset, Device Speed might change so Control Endpoint needs to
            // be recreated.
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "[USBD] USB VBUS connected when reset");
            if (_ux_device_stack_disconnect() != 0U) {
                // (todo) shall we check this?
            }
        }
    }
    isr_usb_reset_notify();
}

static void isr_handler_set_interface(void)
{
    UINT32 intf_index;
    UINT32 alt_index;

    intf_index  = AmbaRTSL_UsbGetDevStatusIntf();
    alt_index   = AmbaRTSL_UsbGetDevStatusAlt();

    if (_ux_device_stack_alternate_setting_set(intf_index, alt_index) != 0U) {
        // (todo) shall we check this?
    }

    AmbaRTSL_UsbSetEp20Interface(0, AmbaRTSL_UsbGetDevStatusIntf());
    AmbaRTSL_UsbSetEp20AltSetting(0, AmbaRTSL_UsbGetDevStatusAlt());
    AmbaRTSL_UsbSetDevCtlCsrdone(1);
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_SI);  // ack to clear

    // Notify ISR task for further process.
    isr_set_interface_notify(intf_index, alt_index);
}

static void isr_handler_set_configuration(const USBD_UDC_s * Udc)
{
    const UDC_ENDPOINT_INFO_s *udc_ep_info;
    UINT32 config_index;

    // since SET_CONFIG is handled by hardware
    // no packet to notify USBX this event
    // so we need to handle this event with UDC's way

    udc_ep_info   = &Udc->DcdEndpointsInfoArray[0];
    config_index  = AmbaRTSL_UsbGetDevStatusCfg();

    if (config_index == 0U) {
        // pollo - 2016/01/19 - Passing USBCV20.
        // DO NOT do anything since USBX can't handle it well.
        // Keep USBX in configuration 1 is easier.
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "[USBD] Set Configureation 0");
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "[USBD] Set Configureation 1");
        if (_ux_device_stack_configuration_set(config_index) != 0U) {
            // (todo) shall we handle this?
        }
        if ((udc_ep_info->EpState & USBD_UDC_ED_STATUS_USED) == 0U) {
            // When doing USBCV test, there's chance enum comes after set-config.
            //   Therefore we call CompleteInit if set-config=1 comes and ED0-STATUS = UNUSED.
            //   We call CompleteInit if Enum-done comes and ED0-STATUS = UNUSED. */
            if (USB_DrvInitComplete() != USB_ERR_SUCCESS) {
                // (todo) shall we handle this?
            }
            // flush current TX FIFO to prevent garbage
            AmbaRTSL_UsbSetEpInCtrlF(0, 1);
        }
    }

    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_SC);  // ack to clear
    AmbaRTSL_UsbSetEpInStall(0, 0);
    AmbaRTSL_UsbSetDevCtlCsrdone(1);

    // Notify the Vbus task configure has been notified.
    USBD_ConnectChgCfgNotifiedSet(1);
    // Notify the upper layer the configure rquest is received.
    isr_set_configure_notify(config_index);
}

static void isr_handler_idle(void)
{
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_ES);
    // do nothing
}

static void isr_handler_sof(void)
{
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_SOF);  // ack to clear
    // should only work with iso endpoint application

    // only for resume detection. Ax chips don't support resume detection.
    // so we need to monitor SOF for it.
    isr_usb_resume_notify();

    // disable SOF Interrupt
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_SOF, 1);
}

static void isr_handler_enumeration(const USBD_UDC_s *Udc, UX_SLAVE_DEVICE *UsbxDevice)
{
    const UDC_ENDPOINT_INFO_s *udc_ep_info;

    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_ENUM);                                        // write to clear

    if (AmbaRTSL_UsbGetDevStatusEnumSpd() == (UINT32)USB_RTSL_DEV_SPD_HI) {        /* high speed */
        _ux_system_slave->ux_system_slave_speed = UX_HIGH_SPEED_DEVICE;
    } else if (AmbaRTSL_UsbGetDevStatusEnumSpd() == (UINT32)USB_RTSL_DEV_SPD_FU) { /* full speed */
        _ux_system_slave->ux_system_slave_speed = UX_FULL_SPEED_DEVICE;
    } else {
        _ux_system_slave->ux_system_slave_speed = UX_FULL_SPEED_DEVICE;
    }

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "[USBD] enumerated: speed %d", _ux_system_slave->ux_system_slave_speed, 0, 0, 0, 0);

    // When doing USBCV test, there's chance enum comes after set-config.
    // Therefore we call CompleteInit if set-config=1 comes and ED0-STATUS = UNUSED.
    // We call CompleteInit if Enum-done comes and ED0-STATUS = UNUSED.
    udc_ep_info = &Udc->DcdEndpointsInfoArray[0];
    if ((udc_ep_info->EpState & USBD_UDC_ED_STATUS_USED) == 0U) {
        // Complete the device initialization.
        (void)USB_DrvInitComplete();

        // Mark the device as attached now.
        UsbxDevice->ux_slave_device_state = UX_DEVICE_ATTACHED;
        // flush current TX FIFO to prevent garbage
        AmbaRTSL_UsbSetEpInCtrlF(0, 1);
    }
}

static void isr_handler_suspend(const UX_SLAVE_DEVICE *Device)
{
    AmbaRTSL_UsbClrDevInt(USB_DEV_INT_US); // write 1 to clear

    // Check the status change callback.
    if (_ux_system_slave->ux_system_slave_change_function != UX_NULL) {
        // Inform the application if a callback function was programmed.
        (void)_ux_system_slave->ux_system_slave_change_function(UX_DEVICE_SUSPENDED);
    }

    // If the device is attached or configured, we have a disconnection signal.
    if (Device->ux_slave_device_state != (UINT32)UX_DEVICE_RESET) {
        // Device is reset, the behavior is the same as disconnection.
    }

    isr_usb_suspend_notify();

    // enable SOF Interrupt for resume detection.
    AmbaRTSL_UsbSetDevMaskInt(USB_DEV_INT_SOF, 0);
}

static void isr_device_handler(const USBD_UDC_s *Udc)
{
    USBD_DEV_INT_STATUS_s int_status;
    UINT32 u32_tmp;
    UX_SLAVE_DEVICE *usbx_device;

    // Get the pointer to the device.
    usbx_device = &_ux_system_slave->ux_system_slave_device;

    // Read the interrupt status register from the controller.
    u32_tmp = AmbaRTSL_UsbGetDevIntStatus();
    USB_UtilityMemoryCopy(&int_status, &u32_tmp, sizeof(UINT32));

    if (int_status.UsbReset == 1U) {       // usb reset
        isr_handler_reset(Udc);
    }

    if (int_status.SetInterface == 1U) {   // set interface
        isr_handler_set_interface();
    }

    if (int_status.SetConfig == 1U) {      // set config
        isr_handler_set_configuration(Udc);
    }

    if (int_status.Empty == 1U) {          // 3ms idle
        isr_handler_idle();
    }

    if (int_status.Sof == 1U) {            // sof interrupt
        isr_handler_sof();
    }

    if (int_status.EnumDone == 1U) {       // the enumeration is done
        isr_handler_enumeration(Udc, usbx_device);
    }

    if (int_status.UsbSuspend == 1U) {     // suspend
        isr_handler_suspend(usbx_device);
    }

    (void)int_status.RemoteWakeUp;
    (void)int_status.Reserved;
}

static void udc_isr_endpoint_out_handler(USBD_UDC_s *Udc, UINT32 EpStatus)
{
    UINT32 int_src = EpStatus;
    UINT32 physical_endpoint_index;
    UX_SLAVE_TRANSFER *transfer_request;
    UDC_ENDPOINT_INFO_s *udc_ep_info;
    UX_SLAVE_ENDPOINT *usbx_endpoint;

    int_src >>= 16;

    for (physical_endpoint_index = 0; physical_endpoint_index <= (UINT32)USBD_UDC_MAX_OUT_ED_NUM; physical_endpoint_index++) {
        // Each endpoint has an interrupt bit in the interrupt status register.
        if ((int_src & (1UL << physical_endpoint_index)) != 0U) {
            const UDC_ENDPOINT_MAP_s *endpoint_map = USBD_UdcEndpointMapGet(Udc, (UINT8)physical_endpoint_index);
            if (endpoint_map == NULL) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "udc_isr_endpoint_out_handler(): can't get Endpoint Map for 0x%X", physical_endpoint_index, 0, 0, 0, 0);
                break;
            }

            // Get the physical endpoint associated with this endpoint.
            udc_ep_info              = &Udc->DcdEndpointsInfoArray[endpoint_map->Udc20Index];
            udc_ep_info->InterruptStatus = AmbaRTSL_UsbGetEpOutStatus(physical_endpoint_index);
            // Get the endpoint to the endpoint.
            usbx_endpoint = udc_ep_info->UxEndpoint;

            if (usbx_endpoint != NULL) {
                // Get the pointer to the transfer request.
                transfer_request = &usbx_endpoint->ux_slave_endpoint_transfer_request;

                // Process the call back.
                USBD_DrvTransferCallback(Udc, USBD_EP_DIR_OUT, physical_endpoint_index, transfer_request);
            }
        }
    }
}

static void udc_isr_endpoint_in_handler(USBD_UDC_s *Udc, UINT32 EpStatus)
{
    UINT32 int_src = EpStatus;
    UINT32 physical_endpoint_index;
    UX_SLAVE_TRANSFER *transfer_request;
    UDC_ENDPOINT_INFO_s *udc_ep_info;
    UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 logical_address;
    const UDC_ENDPOINT_MAP_s *endpoint_map;

    for (physical_endpoint_index = 0; physical_endpoint_index <= (UINT32)USBD_UDC_MAX_IN_ED_NUM; physical_endpoint_index++) {
        // Each endpoint has an interrupt bit in the interrupt status register.
        if ((int_src & (1UL << physical_endpoint_index)) != 0U) {
            logical_address = physical_endpoint_index;
            if (logical_address != 0U) {
                logical_address |= 0x080U;
            }

            endpoint_map = USBD_UdcEndpointMapGet(Udc, (UINT8)logical_address);
            if (endpoint_map == NULL) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "udc_isr_endpoint_in_handler(): can't get Endpoint Map for 0x%X", logical_address, 0, 0, 0, 0);
                break;
            }

            // Get the physical endpoint associated with this endpoint.
            udc_ep_info              = &Udc->DcdEndpointsInfoArray[endpoint_map->Udc20Index];
            udc_ep_info->InterruptStatus = AmbaRTSL_UsbGetEpInStatus(physical_endpoint_index);
            usbx_endpoint = udc_ep_info->UxEndpoint;

            if (usbx_endpoint != NULL) {
                // Get the pointer to the transfer request.
                transfer_request = &usbx_endpoint->ux_slave_endpoint_transfer_request;

                // Process the call back.
                USBD_DrvTransferCallback(Udc, USBD_EP_DIR_IN, physical_endpoint_index, transfer_request);
            }
        }
    }
}

static void isr_endpoint_handler(USBD_UDC_s * Udc)
{
    UINT32 int_src = AmbaRTSL_UsbGetEpInt();

    if (int_src != 0U) {                     // endpoint interrupt
        // ack EP int
        AmbaRTSL_UsbSetEpInt(int_src);

        if ((int_src & 0x00FF0000U) != 0U) { // out endpoint interrupt
            udc_isr_endpoint_out_handler(Udc, int_src);
        }

        if ((int_src & 0x000000FFU) != 0U) { // in endpoint interrupt
            udc_isr_endpoint_in_handler(Udc, int_src);
        }
    }
}

static void udc_isr_handler(USBD_UDC_s *Udc)
{
    isr_device_handler(Udc);
    isr_endpoint_handler(Udc);
}

static void udc_isr_operation(UX_SLAVE_ENDPOINT *Endpoint, UINT32 Opcode, const UINT32 *Params, UINT32 ParamCnt)
{
    UINT32 uret;

    if (ParamCnt > MAX_ISR_OPERATION_PARAM_COUNT) {
        // Shall implement a fault report here considering that the it's called by ISR.
    } else {
        USBD_UDC_s *usbd_udc;
        UDC_ISR_REQUEST_s isr_req;
        UINT32 i;

        uret = USBD_UdcUdcInstanceGet(&usbd_udc);
        if (uret == USB_ERR_SUCCESS) {
            // record current frame number.
            usbd_udc->CurrentFrameNumber = AmbaRTSL_UsbGetDevStatusTs() & 0x7FFU;

            isr_req.Endpoint = Endpoint;
            isr_req.Opcode   = Opcode;

            for (i = 0; i < ParamCnt; i++) {
                isr_req.Params[i] = Params[i];
            }

            uret = USB_UtilityQueueIsrSend(&usbd_udc->IsrRequestQueue, &isr_req, USB_NO_WAIT);
            if (uret != USB_ERR_SUCCESS) {
                // Shall implement a fault report here considering that the it's called by ISR.
            }
        }
    }
}

static void *isr_task_entry(void *Input)
{
    USBD_UDC_s *usbd_udc;
    UINT32 uret;
    UDC_ISR_REQUEST_s isr_req;

    AmbaMisra_TouchUnused(Input);

    USB_UtilityMemorySet(&isr_req, 0, sizeof(UDC_ISR_REQUEST_s));

    uret = USBD_UdcUdcInstanceGet(&usbd_udc);

    if (uret == USB_ERR_SUCCESS) {
        for (;;) {
            usbd_udc->IsrThreadRunning = 1;

            while (usbd_udc->IsrThreadRunning == 1U) {
                UINT32 flag_error = 0;

                uret = USB_UtilityQueueIsrRecv(&usbd_udc->IsrRequestQueue, &isr_req, USB_WAIT_FOREVER);
                if (uret == USB_ERR_SUCCESS) {
                    // interrupt should be disabled here
                    if (isr_req.Opcode == ISR_OPERATION_NOTIFY_INTERRUPT) {
                        udc_isr_handler(usbd_udc);
                        USBD_IntEnable();
                    } else {
                        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "isr_task_entry: Unknown operation %d", isr_req.Opcode, 0, 0, 0, 0);
                        flag_error = 1;
                    }
                } else {
                    flag_error = 1;
                }

                if (flag_error == 1U) {
                    break;
                }
            }
            // Per Rio's request, no task creation in driver and no task delection.
            // Suspend the task.
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "isr_task_entry(): suspend");
            if (USB_UtilityTaskSuspendSelf() != USB_ERR_SUCCESS) {
                // process this error
            }
        }
    }

    // (todo) Do system error reports here.
    return NULL;
}

static void wait_isr_operation_empty(USBD_UDC_s *Udc)
{
    UINT32 retry_count = 0;
    AMBA_KAL_MSG_QUEUE_INFO_s queue_info;

    do {
        UINT32 flag_leave = 0;
        // should be thread context
        if (AmbaKAL_MsgQueueQuery(&Udc->IsrRequestQueue, &queue_info) == 0U) {
            if (queue_info.NumEnqueued == 0U) {
                flag_leave = 1;
            } else {
                USB_UtilityTaskSleep(100);
                retry_count++;
            }
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "wait_isr_operation_empty: can't get ISR Queue Information");
            flag_leave = 1;
        }

        if (flag_leave == 1U) {
            break;
        }
    } while (retry_count < 20U);
}

static UINT32 udc_isr_init(void)
{
    USBD_UDC_s *usbd_udc;
    UINT32 uret;

    uret = USBD_UdcUdcInstanceGet(&usbd_udc);
    if (uret == USB_ERR_SUCCESS) {
        // Per Rio's request, no task creation in driver.
        // the Task is created by applications and we just provide entry function.
        uret = usbd_sys_config_int_local->SystemTaskCreateCb(USBD_ISR_TASK_CODE, isr_task_entry, NULL, &usbd_udc->IsrThread);
        if (uret == USB_ERR_SUCCESS) {
            flag_int_init |= USBD_FLAG_USBD_INT_TASK_INIT;
        } else {
            // Shall not be here.
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ISR Task Create Fail");
            uret = USB_ERR_ISR_TASK_CREATE_FAIL;
        }
    }

    return uret;
}

static UINT32 udc_isr_deinit(void)
{
    USBD_UDC_s *usbd_udc;
    UINT32 uret;
    UDC_ISR_REQUEST_s isr_request;

    uret = USBD_UdcUdcInstanceGet(&usbd_udc);
    if (uret == USB_ERR_SUCCESS) {
        // Wait till all isr is done.
        wait_isr_operation_empty(usbd_udc);

        // Per Rio's request, no task deletion.
        // just tell Isr task to suspend
        if ((flag_int_init & USBD_FLAG_USBD_INT_TASK_INIT) != 0U) {
            usbd_udc->IsrThreadRunning = 0;
            isr_request.Opcode        = ISR_OPERATION_NOTIFY_EXIT;
            uret                      = USB_UtilityQueueIsrSend(&usbd_udc->IsrRequestQueue, &isr_request, 1000);
            if (uret == USB_ERR_SUCCESS) {
                uret = usbd_sys_config_int_local->SystemTaskSuspendCb(USBD_ISR_TASK_CODE, isr_task_entry, usbd_udc->IsrThread);
                if (uret == USB_ERR_SUCCESS) {
                    //flag_int_init &= ~USBD_FLAG_USBD_INT_TASK_INIT;
                } else {
                    // Shall not be here.
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ISR Task suspend Fail");
                    uret = USB_ERR_ISR_TASK_SUSPEND_FAIL;
                }
            } else {
                // error handling
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "udc_isr_deinit(): can't send ISR request.");
            }
        }

        // no need to delete ISR Queue

    } else {
        // error handling
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ISR Task Delete when UDC does not exist!");
    }

    return uret;
}

// disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void udc_isr_entry(UINT32 IntID, UINT32 UserArg)
{

    (void)IntID;
    (void)UserArg;

    // Disable the interrupt.
    USBD_IntDisable();

    // wakeup ISR entry.
    udc_isr_operation(NULL, ISR_OPERATION_NOTIFY_INTERRUPT, NULL, 0);
}

#pragma GCC pop_options

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to check if USB interrupt is enabled.
 * */
UINT32 USBD_IntIsEnabled(void)
{
    UINT32 uret;
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();

    if (AmbaRTSL_USBIsIsrEnable(hws->Udc.IntPin) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}

/**
 * Called by driver to enable all USB interrupts.
 * */
void USBD_IntEnable(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    (void)AmbaRTSL_USBIsrEnable(hws->Udc.IntPin);
}

/**
 * Called by driver to disable all USB interrupts.
 * */
void USBD_IntDisable(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    (void)AmbaRTSL_USBIsrDisable(hws->Udc.IntPin);
}

/**
 * Called by driver to initialize interrupt mechanism
 * */
UINT32 USBD_IntInit(void)
{
    UINT32 uret;

    uret = USBD_SystemConfigGet(&usbd_sys_config_int_local);
    if (uret == USB_ERR_SUCCESS) {
        // ISR init.
        uret = udc_isr_init();
        if (uret == USB_ERR_SUCCESS) {
            const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
            // Hook USB ISR.
            if (AmbaRTSL_USBIsrHook(hws->Udc.IntPin,
                                    hws->Udc.IntType,
                                    udc_isr_entry) != 0U) {
                // shall not be here.
            }
            USBD_IntEnable();
        }
    }

    return uret;
}

/**
 * Called by driver to de-initialize interrupt mechanism
 * */
UINT32 USBD_IntDeInit(void)
{
    UINT32 uret = udc_isr_deinit();

    // We should disable the interrupt after the ISR bottom-half
    // task which could enable it again is deleted.
    USBD_IntDisable();
    return uret;
}
/** @} */
