/**
 *  @file AmbaUSBD_Interrupt.c
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
#include <AmbaRTSL_GIC.h>

#define USBD_FLAG_USBD_INT_TASK_INIT ((UINT32)0x00000001UL)

static UINT32 flag_int_init           = 0;
static UINT32 flag_int_stop           = 0;
static UINT32 babble_count            = 0;
static USBD_SYS_CONFIG_s *usbd_sys_config_int_local = NULL;

static UINT32 udc_isr_deinit(void);

static void udc_isr_handler(USBD_UDC_s *Udc)
{
    UINT32 func_uret;

    AmbaMisra_TouchUnused(Udc);

    func_uret = USBDCDNS3_IsrHandler();
    if (func_uret == USB_ERR_XHCI_EVT_BABBLE) {
        // many babble for events. disable Interrupt to avoid interrupt flooding.
        babble_count++;
        if (babble_count > 100U) {
            USBD_SystemAppEventNotify(USB_ERR_XHCI_EVT_BABBLE, 0, 0, 0);
            flag_int_stop = 1;
        }
    }
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
            //usbd_udc->CurrentFrameNumber = AmbaRTSL_UsbGetDevStatusTs() & 0x7FFU;

            isr_req.Endpoint = Endpoint;
            isr_req.Opcode   = Opcode;

            for (i = 0; i < ParamCnt; i++) {
                isr_req.Params[i] = Params[i];
            }

            uret = USB_UtilityQueueIsrSend(&usbd_udc->IsrRequestQueue, &isr_req, USB_NO_WAIT);
            if (uret != USB_ERR_SUCCESS) {
                // Shall implement a fault report here considering that the it's called by ISR.
                flag_int_stop = 1;
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
                        if (flag_int_stop == 0U) {
                            USBD_IntEnable();
                        } else {
                            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "isr_task_entry: stop because interrupt flooding.");
                        }
                    } else {
                        USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "isr_task_entry: Unknown operation %d", isr_req.Opcode);
                        USBD_IntDisable();
                        //flag_error = 1;
                    }
                } else {
                    USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "isr_task_entry: queue receive failed, code 0x%X", uret);
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
            flag_int_stop = 0;
            babble_count  = 0;
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

static void udc_isr_entry(UINT32 IntID, UINT32 UserArg)
{
    (void)IntID;
    (void)UserArg;

    // Disable the interrupt.
    USBD_IntDisable();

    // wakeup ISR entry.
    udc_isr_operation(NULL, ISR_OPERATION_NOTIFY_INTERRUPT, NULL, 0);
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to check if USB interrupt is enabled.
 * */
UINT32 USBD_IntIsEnabled(void)
{
    AMBA_INT_INFO_s pIntInfo;
    UINT32 uret = 0U;
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();

    if (AmbaRTSL_GicGetIntInfo(hws->Udc32.IntPin, &pIntInfo) == 0U) {
        uret = pIntInfo.IrqEnable;
    }
    return uret;
}

/**
 * Called by driver to enable all USB interrupts.
 * */
void USBD_IntEnable(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    if (AmbaRTSL_GicIntEnable(hws->Udc32.IntPin) != 0U) {
        // action TBD
    }
}

/**
 * Called by driver to disable all USB interrupts.
 * */
void USBD_IntDisable(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    if (AmbaRTSL_GicIntDisable(hws->Udc32.IntPin) != 0U) {
        // action TBD
    }
}

/**
 * Called by driver to initialize interrupt mechanism
 * */
UINT32 USBDCDNS3_IntInit(void)
{
    UINT32 uret;
    UINT32 func_uret;

    uret = USBD_SystemConfigGet(&usbd_sys_config_int_local);
    if (uret == USB_ERR_SUCCESS) {
        // ISR init.
        uret = udc_isr_init();
        if (uret == USB_ERR_SUCCESS) {

            // Hook USB ISR.
            const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
            AMBA_INT_CONFIG_s pIntConfig;

            USBD_IntDisable();

            pIntConfig.TriggerType = hws->Udc32.IntType;
            pIntConfig.IrqType = INT_TYPE_FIQ;  /* Since Linux does not suport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
            pIntConfig.CpuTargets = 0x01;

            func_uret = AmbaRTSL_GicIntConfig(hws->Udc32.IntPin, &pIntConfig, udc_isr_entry, 0);

            if (func_uret != 0U) {
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "USBDCDNS3_IntInit(): ISR config fail, 0x%X", func_uret);
                uret = func_uret;
            }

            if (uret == 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USBDCDNS3_IntInit(): ISR Hooked.");
                USBD_IntEnable();
            }
        }
    }

    return uret;
}

/**
 * Called by driver to de-initialize interrupt mechanism
 * */
UINT32 USBDCDNS3_IntDeInit(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    UINT32 uret;
    UINT32 func_uret;

    // We should disable the interrupt before asking ISR bottom-half task to stop
    USBD_IntDisable();

    uret = udc_isr_deinit();

    // unhook ISR function
    func_uret = AmbaRTSL_GicSetISR(hws->Xhci.IntPin, NULL);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "USBDCDNS3_IntDeInit(): ISR set fail, 0x%X", func_uret);
    }

    return uret;
}
 /** @} */
