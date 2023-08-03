/**
 *  @file AmbaUSBD_Drv_SetupHandler.c
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
 *  @details USB kernel driver setup request handler function.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <usbx/ux_device_stack.h>

#define USBD_FLAG_USBD_SETUP_TASK_INIT 0x00000001UL

#define USBD_CONTROL_REQUEST_SET       0x01U
#define USBD_CONTROL_REQUEST_EXIT      0x02U
#define USBD_CONTROL_REQUEST_ALL       (USBD_CONTROL_REQUEST_SET | USBD_CONTROL_REQUEST_EXIT)
#define USBD_CONTROL_REQUEST_PROCESS   0x00010000U

static UINT32 flag_setup_init = 0;

static UINT32 udc_setup_deinit(void);

static void udc_next_request_enable(void)
{
    // reenable RX DMA and clear NAK
    USBD_UdcInitCtrlSetupDesc();
    if (AmbaRTSL_UsbGetEpOutNAK(0) != 0U) {
        AmbaRTSL_UsbClearEpOutNAK(0, 1);
    }
    AmbaRTSL_UsbEnOutEpInt(0);
    AmbaRTSL_UsbSetEpOutRxReady(0, 1);
    AmbaRTSL_UsbSetDevCtlRde(1);
}

static UINT32 control_request_process_impl(UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT8 request_type = *TransferRequest->ux_slave_transfer_request_setup;
    UINT8 is_request_in = request_type & (UINT8)UX_REQUEST_IN;
    UINT32 uret;

    // Nak In/Out of EP 0.
    AmbaRTSL_UsbSetEpOutNAK(0, 1);
    AmbaRTSL_UsbSetEpInNAK(0, 1);
    // Handle Data phase of control pipe.
    uret = _ux_device_stack_control_request_process(TransferRequest);

    if (uret == (UINT32)UX_SUCCESS) {
        // Handle Status Phase.
        if (is_request_in != 0U) {
            // control-in request
            // receive a ZLP
            TransferRequest->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_IN;
            uret                                             = _ux_device_stack_transfer_request(TransferRequest, 0, 0);
            udc_next_request_enable();
        } else {
            // control-out request
            /* To make sure driver can receive next setup packet in time,
             * reenable RX DMA and clear NAK before handle status phase.
             * Or, overflow condition may hit, since driver miss the setup packet
             * and then protocol error.
             * Only change flow for request_out, or status phase may be corrupted.*/
            /* setup
             *
             * out
             *     ---> reenable here
             * in
             */
            // reenable RX DMA and clear NAK
            udc_next_request_enable();
            // send a ZLP
            TransferRequest->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_OUT;
            uret                                             = _ux_device_stack_transfer_request(TransferRequest, 0, 0);
        }
    }

    return uret;
}

static void print_setup_packet(const UX_SLAVE_TRANSFER *TransferRequest)
{
    USB_UtilityPrint(USB_PRINT_FLAG_L1, "[SETUP PKT]");
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   bmRequestType: 0x%X", TransferRequest->ux_slave_transfer_request_setup[0],
                          0, 0, 0, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   bmRequest:  0x%X", TransferRequest->ux_slave_transfer_request_setup[1],
                          0, 0, 0, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   wValue:      0x%X %X", TransferRequest->ux_slave_transfer_request_setup[3],
                          TransferRequest->ux_slave_transfer_request_setup[2],
                          0, 0, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   wIndex:      0x%X %X", TransferRequest->ux_slave_transfer_request_setup[5],
                          TransferRequest->ux_slave_transfer_request_setup[4],
                          0, 0, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   wLength:     0x%X %X", TransferRequest->ux_slave_transfer_request_setup[7],
                          TransferRequest->ux_slave_transfer_request_setup[6],
                          0, 0, 0);
}

static void control_request_process(const USBD_UDC_s *Udc)
{
    UINT32 uret;

    const UDC_ENDPOINT_INFO_s *udc_ed     = &Udc->DcdEndpointsInfoArray[0];
    UX_SLAVE_ENDPOINT *usbx_endpoint      = udc_ed->UxEndpoint;
    UX_SLAVE_TRANSFER *transfer_request   = NULL;

    if (usbx_endpoint != NULL) {
        transfer_request = &usbx_endpoint->ux_slave_endpoint_transfer_request;

#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
        print_setup_packet(transfer_request);
#endif
#endif

        uret = control_request_process_impl(transfer_request);

        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "udc_setup_task_entry: Error when process Control Request, code 0x%X.",
                                  uret, 0, 0, 0, 0);
            print_setup_packet(transfer_request);
        }
    }
}

static void control_request_loop(USBD_UDC_s *Udc)
{
    UINT32 uret;

    Udc->CtrlReqThreadRunning = 1;
    while (Udc->CtrlReqThreadRunning == 1U) {
        UINT32 actual_flag;
        UINT32 leave = 0;

        uret = USB_UtilityEventRecv(&Udc->CtrlReqEventFlag,
                                    USBD_CONTROL_REQUEST_ALL,
                                    TX_OR_CLEAR,
                                    &actual_flag,
                                    USB_WAIT_FOREVER);

        if (uret == USB_ERR_SUCCESS) {
            if ((actual_flag & USBD_CONTROL_REQUEST_SET) != 0U) {
                uret = USB_UtilityEventSend(&Udc->CtrlReqEventFlag, USBD_CONTROL_REQUEST_PROCESS);
                if (uret == USB_ERR_SUCCESS) {
                    control_request_process(Udc);
                } else {
                    leave = 1;
                }
            }

            if ((actual_flag & USBD_CONTROL_REQUEST_EXIT) != 0U) {
                leave = 1;
            }
        } else {
            leave = 1;
        }
        if (leave == 1U) {
            break;
        }
    }
}
static void *control_request_task_entry(void *Input)
{
    USBD_UDC_s *usb_udc;
    UINT32 uret;

    AmbaMisra_TouchUnused(Input);

    uret = USBD_UdcUdcInstanceGet(&usb_udc);
    if (uret == USB_ERR_SUCCESS) {
        for (;;) {
            control_request_loop(usb_udc);
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "control_request_task_entry(): suspend");
            // Per Rio's request, no task creation in driver and no task delection
            // Suspend the task.
            if (USB_UtilityTaskSuspendSelf() != USB_ERR_SUCCESS) {
                // process this error
            }
        }
    }
    return NULL;
}

static UINT32 udc_setup_init(void)
{
    USBD_UDC_s *usb_udc;
    UINT32 uret;
    USBD_SYS_CONFIG_s *usbd_sys_config = NULL;

    uret = USBD_UdcUdcInstanceGet(&usb_udc);

    if ((USBD_SystemConfigGet(&usbd_sys_config) == USB_ERR_SUCCESS) && (uret == USB_ERR_SUCCESS)) {
        // Per Rio's request, no task creation in driver.
        // the Task is created by applications and we just provide entry function.
        uret = usbd_sys_config->SystemTaskCreateCb(USBD_CONTROL_REQUEST_TASK_CODE, control_request_task_entry, NULL, &usb_udc->CtrlReqThread);
        if (uret == USB_ERR_SUCCESS) {
            flag_setup_init |= USBD_FLAG_USBD_SETUP_TASK_INIT;
        }
    }

    return uret;
}

static UINT32 udc_setup_deinit(void)
{
    USBD_UDC_s *usb_udc;
    UINT32 uret;
    USBD_SYS_CONFIG_s *usbd_sys_config = NULL;

    uret = USBD_UdcUdcInstanceGet(&usb_udc);
    if ((USBD_SystemConfigGet(&usbd_sys_config) == USB_ERR_SUCCESS) && (uret == USB_ERR_SUCCESS)) {
        // Per Rio's request, no task creation in driver.
        // the Task is created by applications and we just provide entry function.

        if ((flag_setup_init & USBD_FLAG_USBD_SETUP_TASK_INIT) != 0U) {
            // just tell Control Request task to stop.
            // Application should terminate and delete it.
            usb_udc->CtrlReqThreadRunning = 0;
            uret                          = USB_UtilityEventSend(&usb_udc->CtrlReqEventFlag, USBD_CONTROL_REQUEST_EXIT);
            if (uret == USB_ERR_SUCCESS) {
                uret = usbd_sys_config->SystemTaskSuspendCb(USBD_CONTROL_REQUEST_TASK_CODE, control_request_task_entry, usb_udc->CtrlReqThread);
                if (uret == USB_ERR_SUCCESS) {
                    //flag_setup_init &= ~USBD_FLAG_USBD_SETUP_TASK_INIT;
                } else {
                    uret = USB_ERR_CTRL_TASK_SUSPEND_FAIL;
                }
            }
        }
    } else {
        // error case
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "Setup handler Task Delete when UDC does not exist!");
    }

    return uret;
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by ISR task to ask Control-Request task processing USB setup packet.
 * It is synchronous call.
 * */
void USBD_SetupCtrlRequestNotify(void)
{
    USBD_UDC_s *usb_udc;
    UINT32 uret;
    UINT32 actual_flag;

    uret = USBD_UdcUdcInstanceGet(&usb_udc);
    if (uret == USB_ERR_SUCCESS) {
        // set event to start Control Transfer
        uret = USB_UtilityEventSend(&usb_udc->CtrlReqEventFlag, USBD_CONTROL_REQUEST_SET);
        if (uret != USB_ERR_SUCCESS) {
            // shall not be here.
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_SetupCtrlRequestNotify: can't set event flags 0x%x", uret, 0, 0, 0, 0);
        }

        // wait until Control Transfer is processing
        uret = USB_UtilityEventRecv(
                   &usb_udc->CtrlReqEventFlag,
                   USBD_CONTROL_REQUEST_PROCESS,
                   TX_AND_CLEAR,
                   &actual_flag,
                   3000);
        if ((uret != USB_ERR_SUCCESS) || (actual_flag != USBD_CONTROL_REQUEST_PROCESS)) {
            // error handling
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_SetupCtrlRequestNotify: can't wait wait done event flags, code 0x%X, flag 0x%X.", uret, actual_flag, 0, 0, 0);
        }
    }
}

/**
 * Initialize resources for handling USB setup packets
 * */
UINT32 USBD_SetupInit(void)
{
    // Init setup handler.
    return udc_setup_init();
}

/**
 * De-Initialize resources for handling USB setup packets
 * */
UINT32 USBD_SetupDeInit(void)
{
    UINT32 uRet = udc_setup_deinit();
    if (uRet != USB_ERR_SUCCESS) {
        // shall not be here.
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "setup handler resource release fail");
    }
    return uRet;
}
/** @} */
