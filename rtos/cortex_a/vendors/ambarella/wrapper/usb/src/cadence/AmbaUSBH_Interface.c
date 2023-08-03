/**
 *  @file AmbaUSBH_Interface.c
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
 *  @details Ambarella USB Host Driver for UX and Ambarella interface
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaRTSL_GIC.h>

#include <ux_hcd_xhci.h>

#define UX_XHCI_CONTROLLER                                  ((UINT)3U)

static UX_HCD_XHCI g_hcd_xhci;

static void xhci_int_disable(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    if (AmbaRTSL_GicIntDisable(hws->Xhci.IntPin) != 0U) {
        // action TBD
    }
}

static void xhci_int_enable(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    if (AmbaRTSL_GicIntEnable(hws->Xhci.IntPin) != 0U) {
        // action TBD
    }
}

static void ux_hcd_connection_changed(UINT32 PortIndex)
{
    g_hcd_xhci.parent_hcd->ux_hcd_root_hub_signal[PortIndex]++;

    if (_ux_utility_semaphore_put(&_ux_system_host->ux_system_host_enum_semaphore) != 0U) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_connection_changed(): fail to give semaphore");
    }
}

/**
 * Command dispatch and handling function called by USBX
 */
UINT  ux_hcd_xhci_entry(UX_HCD *hcd, UINT function, void *parameter)
{

    UINT               status = UX_FUNCTION_NOT_SUPPORTED;
    UX_HCD_XHCI       *ptr_hcd_xhci;
    XHC_CDN_DRV_RES_s *drv_res = NULL;
    UINT32             port_id;
    UX_ENDPOINT       *ptr_ux_endpoint;
    UX_TRANSFER       *ptr_ux_transfer;
    ULONG             *ptr_ulong;
    ULONG              value_ulong;

    AmbaMisra_TouchUnused(hcd);
    AmbaMisra_TouchUnused(parameter);

    // Check the status of the controller.
    if (hcd->ux_hcd_status == (UINT)UX_UNUSED) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): hcd not used");
        status = (UX_CONTROLLER_UNKNOWN);
    } else {

        // Get the pointer to the EHCI HCD.
        // expansion: ptr_hcd_xhci =  (UX_HCD_XHCI *) hcd->ux_hcd_controller_hardware;
        AmbaMisra_TypeCast(&ptr_hcd_xhci, &hcd->ux_hcd_controller_hardware);

        if (ptr_hcd_xhci != NULL) {
            drv_res = ptr_hcd_xhci->cdns_drv_res;
        }

        // Look at the function and route it.
        switch(function) {

            case UX_HCD_DISABLE_CONTROLLER:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_DISABLE_CONTROLLER not supported");
                //status =  _ux_hcd_ehci_controller_disable(hcd_ehci);
                break;

            case UX_HCD_GET_PORT_STATUS:
                // expansion: value_ulong = (ULONG)parameter;
                AmbaMisra_TypeCast(&value_ulong, &parameter);
                port_id = (UINT32)value_ulong;
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_INFO, "ux_hcd_xhci_entry(): Get Port %d Status", port_id);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    status = ux_hcd_xhci_port_status_get(drv_res, port_id);
                } else {
                    status = UX_PORT_INDEX_UNKNOWN;
                }
                break;

            case UX_HCD_ENABLE_PORT:
                // expansion: value_ulong = (ULONG)parameter;
                AmbaMisra_TypeCast(&value_ulong, &parameter);
                port_id = (UINT32)value_ulong;
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_INFO, "ux_hcd_xhci_entry(): Enable Port %d", port_id);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    status = USBHCDNS3_SlotEnable(drv_res, 1);
                } else {
                    status = UX_PORT_INDEX_UNKNOWN;
                }
                break;

            case UX_HCD_DISABLE_PORT:
                AmbaMisra_TypeCast(&value_ulong, &parameter);
                port_id = (UINT32)value_ulong;
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_INFO, "ux_hcd_xhci_entry(): Disable Port %d not supported.", port_id);
                //status =  _ux_hcd_ehci_port_disable(hcd_ehci, (ULONG) parameter);
                break;

            case UX_HCD_POWER_ON_PORT:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_POWER_ON_PORT not supported");
                //status =  _ux_hcd_ehci_power_on_port(hcd_ehci, (ULONG) parameter);
                break;


            case UX_HCD_POWER_DOWN_PORT:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_POWER_DOWN_PORT not supported");
                //status =  _ux_hcd_ehci_power_down_port(hcd_ehci, (ULONG) parameter);
                break;


            case UX_HCD_SUSPEND_PORT:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_SUSPEND_PORT not supported");
                //status =  _ux_hcd_ehci_port_suspend(hcd_ehci, (ULONG) parameter);
                break;


            case UX_HCD_RESUME_PORT:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_RESUME_PORT not supported");
                //status =  _ux_hcd_ehci_port_resume(hcd_ehci, (UINT) parameter);
                break;


            case UX_HCD_RESET_PORT:
                // expansion: value_ulong = (ULONG)parameter;
                AmbaMisra_TypeCast(&value_ulong, &parameter);
                port_id = (UINT32)value_ulong;
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_INFO, "ux_hcd_xhci_entry(): Reset Port %d", port_id);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    status =  ux_hcd_xhci_port_reset(drv_res, port_id);
                } else {
                    status = UX_PORT_RESET_FAILED;
                }
                break;

            case UX_HCD_GET_FRAME_NUMBER:
                USB_UtilityPrint(USB_PRINT_FLAG_L4, "ux_hcd_xhci_entry(): Get Frame Number.");
                // expansion: ptr_ulong = ((ULONG *)parameter);
                AmbaMisra_TypeCast(&ptr_ulong, &parameter);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    status = ux_hcd_xhci_frame_number_get(drv_res, ptr_ulong);
                } else {
                    // if controller is stopped, stack may still asking for frame number.
                    // we need to make frame number increasing in this case
                    static ULONG fn = 0;
                    *ptr_ulong = fn;
                    fn++;
                    status = 0;
                }
                break;

            case UX_HCD_SET_FRAME_NUMBER:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_SET_FRAME_NUMBER not supported");
                //_ux_hcd_ehci_frame_number_set(hcd_ehci, (ULONG) parameter);
                //status =  UX_SUCCESS;
                break;

            case UX_HCD_TRANSFER_REQUEST:
                // expansion: ptr_ux_transfer = (UX_TRANSFER *) parameter
                AmbaMisra_TypeCast(&ptr_ux_transfer, &parameter);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    status =  ux_hcd_xhci_request_transfer(ptr_hcd_xhci, ptr_ux_transfer);
                } else {
                    status = UX_TRANSFER_ERROR;
                }
                break;

            case UX_HCD_TRANSFER_ABORT:
                // expansion: ptr_ux_transfer = (UX_TRANSFER *) parameter
                AmbaMisra_TypeCast(&ptr_ux_transfer, &parameter);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    // only do abort when controller is running
                    status =  ux_hcd_xhci_request_abort(ptr_hcd_xhci, ptr_ux_transfer);
                } else {
                    status = UX_TRANSFER_NOT_READY;
                }
                break;

            case UX_HCD_CREATE_ENDPOINT:
                // expansion: ptr_ux_endpoint = (UX_ENDPOINT *)parameter;
                AmbaMisra_TypeCast(&ptr_ux_endpoint, &parameter);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    status = ux_hcd_xhci_endpoint_create(drv_res, ptr_ux_endpoint);
                } else {
                    status = 0;
                }
                break;


            case UX_HCD_DESTROY_ENDPOINT:
                // expansion: ptr_ux_endpoint = (UX_ENDPOINT *)parameter;
                AmbaMisra_TypeCast(&ptr_ux_endpoint, &parameter);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    // only do it when controller is running
                    status = ux_hcd_xhci_endpoint_destroy(drv_res, ptr_ux_endpoint);
                } else {
                    status = 0;
                }
                break;

            case UX_HCD_RESET_ENDPOINT:
                // expansion: ptr_ux_endpoint = (UX_ENDPOINT *)parameter;
                AmbaMisra_TypeCast(&ptr_ux_endpoint, &parameter);
                if ((ptr_hcd_xhci != NULL) && (ptr_hcd_xhci->flag_running == 1U)) {
                    // only do it when controller is running
                    status =  ux_hcd_xhci_endpoint_reset(drv_res, ptr_ux_endpoint);
                } else {
                    status = 0;
                }
                break;

            case UX_HCD_PROCESS_DONE_QUEUE:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_PROCESS_DONE_QUEUE not supported");
                break;

            case UX_HCD_SET_TEST_MODE:
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): UX_HCD_SET_TEST_MODE not supported");

                //_ux_hcd_ehci_test_mode(hcd_ehci, (UX_HOST_TEST_MODE_INFO*) parameter);
                //status =  UX_SUCCESS;
                break;

            default:
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_entry(): unsupported function id = %d", function);
                status =  UX_FUNCTION_NOT_SUPPORTED;
                break;
        }
    }

    return(status);
}

/**
 * Host controller driver initialization function called by USBX
 * It is register through ux_host_stack_hcd_register()
 */
UINT  ux_hcd_xhci_initialize(UX_HCD *hcd)
{
    UINT               uret;
    const UX_HCD_XHCI *ptr_hcd_xhci = &g_hcd_xhci;

    /* The controller initialized here is of EHCI type.  */
    hcd->ux_hcd_controller_type =  UX_XHCI_CONTROLLER;

    /* zero-out memory for this XHCI HCD instance.  */
    USB_UtilityMemorySet(&g_hcd_xhci, 0, sizeof(UX_HCD_XHCI));

    /* Set the pointer to the EHCI HCD.  */
    // expansion: hcd->ux_hcd_controller_hardware =  (void *)ptr_hcd_xhci;
    AmbaMisra_TypeCast(&hcd->ux_hcd_controller_hardware, &ptr_hcd_xhci);

    /* Save the register memory address.  */
    g_hcd_xhci.base_address =  hcd->ux_hcd_io;

    /* Set the generic HCD owner for the EHCI HCD.  */
    g_hcd_xhci.parent_hcd = hcd;

    /* Get the number of ports on the controller. */
    hcd->ux_hcd_nb_root_hubs =  0x02U;

    /* Initialize the function entry for this HCD.  */
    hcd->ux_hcd_entry_function = ux_hcd_xhci_entry;

    /* Set the state of the controller to HALTED first.  */
    hcd->ux_hcd_status =  UX_HCD_STATUS_HALTED;

    /* assing memory for cadence driver and run the controller */
    uret = USBHCDNS3_ControllerInit(&g_hcd_xhci.cdns_drv_res);

    if (uret == 0U) {
        /* Since this is a USB 3.2 (Gen2) controller, we can safely hardwire the version.  */
        hcd->ux_hcd_version =  0x322;

        /* Set the state of the controller to OPERATIONAL.  */
        hcd->ux_hcd_status = UX_HCD_STATUS_OPERATIONAL;

        g_hcd_xhci.cdns_drv_res->HostCallbacks.ConnectionChanged = ux_hcd_connection_changed;

        g_hcd_xhci.flag_running = 1;

        xhci_int_enable();
    }

    return(uret);
}

static void phy_enable(UINT32 PowerOn)
{
    if (PowerOn == 1U) {
        // Configure PHY and let it run
        USBCDNS3_PHYUp();
    } else {
        // put PHY in reset state
        USBCDNS3_PHYDown();
    }
}

static void uxh_xhci_interrupt_handler(void)
{
    // interrupt should be disabled now

    // call ISR handler
    UINT32 uret = USBHCDNS3_Isr();
    if (uret != 0U) {
        // action TBD
    }

    // enable interrupt
    xhci_int_enable();

}

static AMBA_KAL_EVENT_FLAG_t        xhci_event_flag;
#define USBH_EVTFLG_INTERRUPT       0x01U
#define USBH_EVTFLG_ALL             (USBH_EVTFLG_INTERRUPT)

// disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void xhci_isr_entry(UINT32 IntID, UINT32 UserArg)
{
    (void)IntID;
    (void)UserArg;

    // disable interrupt
    xhci_int_disable();

    if (USB_UtilityEventSend(&xhci_event_flag, USBH_EVTFLG_INTERRUPT) != 0U) {
        // action TBD
    }
}
#pragma GCC pop_options

#define XHCI_TASK_PRIORITY (10U)
#define XHCI_TASK_STKSZ    (1024U*16U)

static void *xhci_isr_task(void *Arg)
{
    UINT32 func_uret;
    UINT32 flag_leave = 0;
    UINT32 actual_flag;

    AmbaMisra_TouchUnused(Arg);

    while (flag_leave == 0U) {
        func_uret = USB_UtilityEventRecv(&xhci_event_flag,
                                        USBH_EVTFLG_ALL,
                                        USB_KAL_OR_CLEAR,
                                        &actual_flag,
                                        USB_WAIT_FOREVER);
        if (func_uret != 0U) {
            flag_leave = 1;
        } else {
            if ((actual_flag & USBH_EVTFLG_INTERRUPT) != 0U) {
                uxh_xhci_interrupt_handler();
            }
        }
    }
    return NULL;
}

static AMBA_KAL_TASK_t xhci_task;

static void interrupt_init(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    AMBA_INT_CONFIG_s pIntConfig;
    UINT32 func_uret;
    static char  xhci_task_name[] = "xhci_task";
    static UINT8 xhci_task_stksz[XHCI_TASK_STKSZ];

    func_uret = USB_UtilityEventCreate(&xhci_event_flag);

    if (func_uret == 0U) {
        func_uret = USB_UtilityTaskCreate(
                            &xhci_task,
                            xhci_task_name,
                            XHCI_TASK_PRIORITY,
                            xhci_isr_task,
                            NULL,
                            xhci_task_stksz,
                            XHCI_TASK_STKSZ,
                            USB_KAL_AUTO_START,
                            0x01);
        if (func_uret != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "interrupt_init(): can't create xhci isr task.");
        }
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "interrupt_init(): can't create xhci event flag.");
    }

    if (func_uret == 0U) {
        pIntConfig.TriggerType = hws->Xhci.IntType;
        pIntConfig.IrqType = INT_TYPE_FIQ;  /* Since Linux does not suport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
        pIntConfig.CpuTargets = 0x01;

        func_uret = AmbaRTSL_GicIntConfig(hws->Xhci.IntPin, &pIntConfig, xhci_isr_entry, 0);
        if (func_uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "interrupt_init(): fail to config ISR(%d), 0x%X",
                                  hws->Xhci.IntPin, func_uret, 0, 0, 0);
        }
    }
}

static void interrupt_deinit(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    UINT32 func_uret;

    xhci_int_disable();

    // unhook ISR function
    func_uret = AmbaRTSL_GicSetISR(hws->Xhci.IntPin, NULL);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "interrupt_deinit(): ISR set fail, 0x%X", func_uret);
    }

    // delete task
    func_uret = USB_UtilityTaskDelete(&xhci_task, 0);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "interrupt_deinit(): fail to delete task, 0x%X", func_uret);
    } else {
        USB_UtilityMemorySet(&xhci_task, 0, sizeof(xhci_task));
    }

    // delete event flags
    func_uret = USB_UtilityEventDelete(&xhci_event_flag);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "interrupt_deinit(): fail to delete eventflags, 0x%X", func_uret);
    } else {
        USB_UtilityMemorySet(&xhci_event_flag, 0, sizeof(xhci_event_flag));
    }

}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to initialize Host controller.
 * It configures related hardware registers exept UHCI/EHCI.
 * UHCI/EHCI controller is handled by USBX.
*/
void USBH_UhcInit(const USBH_CLASS_CONFIG_s *Config)
{
    (void)Config;

    // phy setup
    phy_enable(1);

    // overcurrent polarity.
    // no polarity we can set (for CV5)

    // interrupt setup
    interrupt_init();
}
/**
 * Called by driver to de-initialize Host controller.
 * It configures related hardware registers exept XHCI.
*/
void USBH_UhcDeInit(void)
{
    // deinit interrupt.
    interrupt_deinit();

    // stop XHC controller
    if (USBCDNS3_XhcStop() != 0U) {
        // ignore this error
    }

    // turn off controller
    USBCDNS3_ControllerDown();

    // phy deinit
    phy_enable(0);

    return;
}
/** @} */
