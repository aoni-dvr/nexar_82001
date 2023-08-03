/**
 *  @file AmbaUSBD_Drv_ConnectChange.c
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
 *  @details USB connection change functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

//#define USB_VBUS_TRIGGER_USE_IRQ

typedef struct {
    UINT32           VbusSignal;                        // Current Vbus level.
    UINT32           VbusPreSignal;                     // Previous Vbus level.
    UINT32           Connected;                         // Vbus logically connected.
    UINT32           VbusPeriod;                        // Vbus task polling interval.
    UINT32           ConfigurtionIndexCurr;             // Current set configuration index value.
    UINT32           ConfigurtionIndexPrev;             // Previous set configuration index value.
    UINT32           SetCfgNotified;                    // Whether the application has been informed the event.
    UINT32           SetCfgCntStart;                    // The flag to indicate the set_configure timeout is counting.
    UINT32           SetCfgExpireCnt;                   // The timeout counter to wait set_configure request.
    UINT32           VbusTaskRun;                       // The Vbus detection task is running.
    AMBA_KAL_TASK_t *VbusTask;                          // Vbus detection task.
    AMBA_KAL_SEMAPHORE_t   VbusSema;                    // Vbus semaphore put by ISR.
} USBD_CONN_CHG_s;

#define USBD_FLAG_CONN_CHG_SEMA_INIT  0x00000002UL
#define USBD_FLAG_CONN_CHG_TASK_INIT  0x00000004UL

static USBD_SYS_CONFIG_s *sys_config = NULL;
static USBD_CONN_CHG_s    conn_chg __attribute__((section(".bss.noinit")));
static UINT32             flag_conn_chg_init = 0;

#if defined(USB_VBUS_TRIGGER_USE_IRQ)

static void conn_chg_vbus_interrupt_routine(INT32 IntID)
{
    _ux_utility_semaphore_put(&conn_chg.VbusSema);
}

#endif

static void vbus_signal_update(void)
{
    conn_chg.VbusSignal = AmbaRTSL_UsbVbusVicRawStaus();
}

static void vbus_signal_connect(void)
{

    if (conn_chg.Connected == 0U) {
        conn_chg.Connected = 1;

        if (sys_config != NULL) {
            if (sys_config->SystemEventCallback != NULL) {
                if (sys_config->SystemEventCallback(USBD_SYSTEM_EVENT_CONNECT, 0, 0, 0) != 0U) {
                    // ignore error
                }
            }
        }
    }
}

static void vbus_signal_disconnect(void)
{

    if (conn_chg.Connected != 0U) {
        conn_chg.Connected             = 0;
        conn_chg.ConfigurtionIndexCurr = 0;
        conn_chg.ConfigurtionIndexPrev = 0;
        conn_chg.SetCfgExpireCnt       = 0;
        conn_chg.SetCfgNotified        = 0;
        conn_chg.SetCfgCntStart        = 0;

        if (sys_config != NULL) {
            if (sys_config->SystemEventCallback != NULL) {
                if (sys_config->SystemEventCallback(USBD_SYSTEM_EVENT_DISCONNECT, 0, 0, 0) != 0U) {
                    // ignore error
                }
            }
        }
    }
}


static void vbus_detect_loop(void)
{
    // Make sure to set VIC configuration before vbus task creation,
    // or device may meet the false alarm of vbus detection (especially for SMP).
    const USB_HARDWARE_SETTING_s hws = *USB_UtilityHWInfoQuery();
    AmbaRTSL_UsbVbusVicConfig(hws.Udc.IntPin, hws.Udc.IntType);

    while (conn_chg.VbusTaskRun == 1U) {

        if (USB_UtilitySemaphoreTake(&conn_chg.VbusSema, conn_chg.VbusPeriod) == USB_ERR_SEMA_TAKE_FAIL) {
            // shall not be here.
        }

        vbus_signal_update();

        if ((conn_chg.VbusSignal == 1U) && (conn_chg.VbusPreSignal == 0U)) {
            vbus_signal_connect();
        } else if ((conn_chg.VbusSignal == 0U) && (conn_chg.VbusPreSignal == 1U)) {
            vbus_signal_disconnect();
        } else {
            if ((conn_chg.SetCfgCntStart != 0U) &&
                (conn_chg.Connected != 0U) &&
                (conn_chg.SetCfgNotified == 0U)) {
                // USB cable is inserted. Check Configuration Index
                conn_chg.ConfigurtionIndexCurr = AmbaRTSL_UsbGetDevStatusCfg();
                if (conn_chg.ConfigurtionIndexCurr != conn_chg.ConfigurtionIndexPrev) {
                    if (sys_config != NULL) {
                        if (sys_config->SystemEventCallback != NULL) {
                            if (sys_config->SystemEventCallback(USBD_SYSTEM_EVENT_CONFIGURED, conn_chg.ConfigurtionIndexCurr, 0, 0) != 0U) {
                                // ignore error
                            }
                        }
                    }
                    conn_chg.SetCfgNotified = 1;
                } else {
                    if (sys_config != NULL) {
                        if (conn_chg.SetCfgExpireCnt > sys_config->SetCfgThd) {
                            // index value NOT changed, notify upper layer once.
                            conn_chg.SetCfgNotified = 1;
                            if (sys_config != NULL) {
                                if (sys_config->SystemEventCallback != NULL) {
                                    if (sys_config->SystemEventCallback(USBD_SYSTEM_EVENT_CONFIGURED, 0x0FFFF, 0, 0) != 0U) {
                                        // ignore error
                                    }
                                }
                            }
                        }
                    }
                }
                conn_chg.SetCfgExpireCnt += conn_chg.VbusPeriod;
            }
        }

        conn_chg.ConfigurtionIndexPrev = conn_chg.ConfigurtionIndexCurr;
        conn_chg.VbusPreSignal         = conn_chg.VbusSignal;
    }
}

static void *conn_chg_vbus_det_task_entry(void *Input)
{
    static UINT32 vbus_detection_task_status = USB_ERR_SUCCESS;

    AmbaMisra_TouchUnused(Input);

#if defined(USB_VBUS_TRIGGER_USE_IRQ)
    // Setup the isr of Vbus connect change.
    if (AmbaRTSL_USBIsrHook(AMBA_RTSL_ISR_CONNECT_CHANGE,
                            AMBA_RTSL_ISR_CONNECT_CHANGE,
                            AMBA_RTSL_ISR_CONNECT_CHANGE_TRIGGER_LEVEL,
                            conn_chg_vbus_interrupt_routine) == OK) {
        // Enable the Vbus connect change isr.
        if (AmbaRTSL_USBIsrEnable(AMBA_RTSL_ISR_CONNECT_CHANGE) != OK) {
            // Shall not be here.
            vbus_detection_task_status = USB_ERR_VBUS_ISR_HOOK_FAIL;
        }
    } else {
        // Shall not be here.
        vbus_detection_task_status = USB_ERR_VBUS_ISR_HOOK_FAIL;
    }
#endif

    if (vbus_detection_task_status == USB_ERR_SUCCESS) {
        conn_chg.VbusPeriod      = 100;

        for (;;) {
            conn_chg.VbusTaskRun = 1;

            vbus_detect_loop();

            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "conn_chg_vbus_det_task_entry(): suspend.");
            // Per Rio's request, no task creation in driver.
            // the Task is created by applications and we just provide entry function.
            // Suspend the task.
            if (USB_UtilityTaskSuspendSelf() != USB_ERR_SUCCESS) {
                // process this error
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "conn_chg_vbus_det_task_entry(): suspend failed.");
                break;
            }
        }
    }

    // (todo) Do system error reports here.
    return NULL;
}

static UINT32 conn_chg_free_resource(void)
{
    UINT32 uret = USB_ERR_SUCCESS;

    // Per Rio's request, no task creation in driver.
    // the Task is created by applications and we just provide entry function.
    if ((flag_conn_chg_init & USBD_FLAG_CONN_CHG_TASK_INIT) != 0U) {
        conn_chg.VbusTaskRun = 0;
        if (sys_config != NULL) {
            if (sys_config->SystemTaskSuspendCb != NULL) {
                // wait for vbus polling period to make sure VBUS task can suspend
                USB_UtilityTaskSleep(conn_chg.VbusPeriod * 2U);
                uret = sys_config->SystemTaskSuspendCb(USBD_VBUS_TASK_CODE, conn_chg_vbus_det_task_entry, conn_chg.VbusTask);
                if (uret != USB_ERR_SUCCESS) {
                    uret = USB_ERR_VBUS_TASK_SUSPEND_FAIL;
                } else {
                    //flag_conn_chg_init &= ~USBD_FLAG_CONN_CHG_TASK_INIT;
                }
            }
        }
    }

    // no need to release mutex and semaphore since the task is not deleted.

    return uret;
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Initialize Connection Change Manager.
 * */
UINT32 USBD_ConnectChgInit(void)
{
    UINT32 uret;

    uret = USBD_SystemConfigGet(&sys_config);
    // Sanity check. It won't happen as long as this function is called
    // in AmbaUSBD_System_Init().
    if (uret == USB_ERR_SUCCESS) {
        static UINT32 conn_chg_init = 0;

        if (conn_chg_init == 0U) {
            // just init once
            USB_UtilityMemorySet(&conn_chg, 0, sizeof(USBD_CONN_CHG_s));
            conn_chg_init = 1;
        }

        // Create resource.
        if ((flag_conn_chg_init & USBD_FLAG_CONN_CHG_SEMA_INIT) == 0U) {
            uret = USB_UtilitySemaphoreCreate(&conn_chg.VbusSema, 1);
            if (uret != USB_ERR_SUCCESS) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_ConnectChgInit(): can't create semaphore.");
                uret = USB_ERR_VBUS_SEMA_CREATE_FAIL;
            } else {
                flag_conn_chg_init |= USBD_FLAG_CONN_CHG_SEMA_INIT;
            }
        }

        if (uret == USB_ERR_SUCCESS) {
            // Per Rio's request, no task creation in driver.
            // the Task is created by applications and we just provide entry function.
            if (sys_config != NULL) {
                if (sys_config->SystemTaskCreateCb != NULL) {
                    uret = sys_config->SystemTaskCreateCb(USBD_VBUS_TASK_CODE, conn_chg_vbus_det_task_entry, NULL, &conn_chg.VbusTask);
                } else {
                    uret = USB_ERR_NO_INSTANCE;
                }
            } else {
                uret = USB_ERR_NO_INSTANCE;
            }
            if (uret == USB_ERR_SUCCESS) {
                flag_conn_chg_init |= USBD_FLAG_CONN_CHG_TASK_INIT;
            } else {
                uret = USB_ERR_VBUS_TASK_CREATE_FAIL;
            }
        }

        // wait for task is running?
    }

    // Release resource if any error
    if (uret != USB_ERR_SUCCESS) {
        if (conn_chg_free_resource() != USB_ERR_SUCCESS) {
            // Shall not be here.
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "connect change resource release fail");
        }
    }

    return uret;
}

/**
 * De-initialize Connection Change Manager.
 * */
UINT32 USBD_ConnectChgDeInit(void)
{
    UINT32 uret = conn_chg_free_resource();

    sys_config = NULL;

    return uret;
}
/**
 * Tell Connection Change Manager that "Set Configuration" has been notified to application if the input value is not 0.
 * Thus Connection Change Manager will not notify again.
 * */
void USBD_ConnectChgCfgNotifiedSet(UINT32 Notify)
{
    conn_chg.SetCfgNotified = Notify;
}
/**
 * Tell Connection Change Manager to start counting for "Set Configuration" timeout if the input value is not 0.
 * */
void USBD_ConnectChgCfgCntStartSet(UINT32 Start)
{
    conn_chg.SetCfgCntStart = Start;
}
/**
 * Get current VBUS status.
 * */
UINT32 USBD_ConnectChgVbusStatusGet(void)
{
    UINT32 vbus_signal;

    if ((flag_conn_chg_init & USBD_FLAG_CONN_CHG_TASK_INIT) != 0U) {
        vbus_signal = AmbaRTSL_UsbVbusVicRawStaus();
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_ConnectChgVbusStatusGet cannot be called before AmbaUSBD_System_Init()");
        vbus_signal = 0;
    }

    return vbus_signal;
}
/** @} */
