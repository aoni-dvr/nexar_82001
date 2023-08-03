/**
 *  @file AppUSB.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details USB main application.
 */

#include "AppUSB.h"
#ifndef AMBAUSB_HOST_API_H
#include <AmbaUSBH_API.h>
#endif
#include <AmbaUSB_ErrCode.h>
#include <AmbaGPIO_Def.h>
#include <AmbaGPIO.h>
#include <AmbaMisraFix.h>

#ifndef AMBA_USB_UVCD_H
#include "AppUSB_UVCDevice.h"
#endif

#define TASK_PRI_USBD_VBUS_DETECTION  (AMBA_KAL_TASK_LOWEST_PRIORITY - 2U)
#define TASK_PRI_USBD_CONTROL_REQ     (12U)
#define TASK_PRI_USBD_ISR_REQ         (10U)

#define VBUS_TASK_STACK_SIZE       (16 * 1024)
#define ISR_TASK_STACK_SIZE        (8 * 1024)
#define CTRL_TASK_STACK_SIZE       (8 * 1024)
#define MAX_INTERFACE_CALLBACK     (6U)

UINT8 g_usbx_memory_cached[USBX_CACHE_MEM_SIZE] __attribute__((section(".bss.noinit")));

static APPUSBD_CLR_STALL_CB     callback_clear_stall = NULL;
static APPUSBD_SET_INTERFACE_CB callbacks_set_interface[MAX_INTERFACE_CALLBACK] = {NULL, NULL, NULL, NULL, NULL, NULL};

static AMBA_KAL_TASK_t vbus_detection_task;
static APPUSBD_CLASS_e usbd_current_class_id = APPUSBD_CLASS_MSC;
static UINT32 usb_debug_level = 0xFFFFFFFFU;

void AppUsb_DebugLevelSet(UINT32 Level)
{
    usb_debug_level = Level;
}

UINT32 AppUsb_DebugLevelGet(void)
{
    return usb_debug_level;
}

static void usbd_cb_clear_feature(UINT32 ReqType, UINT32 wValue, UINT32 wIndex)
{
    (VOID)ReqType;
    (VOID)wValue;

    AppUsb_PrintUInt5("USB Device Clr Feature (ReqType = %d, FS = %d, wIdx = 0x%X)", ReqType, wValue, wIndex, 0, 0);
    if (callback_clear_stall != NULL) {
        (VOID)callback_clear_stall(wIndex);
    }
}

static void usbd_cb_set_intface(const UINT32 InterfNum, const UINT32 AltNum)
{
    AppUsb_PrintUInt5("USB Device Set Interface, Interface Num = %d, Alternate Setting = %d", InterfNum, AltNum, 0, 0, 0);
    if (callbacks_set_interface[InterfNum] != NULL) {
        (VOID)callbacks_set_interface[InterfNum](InterfNum, AltNum);
    }
}

static void usbd_cb_error_notify(const UINT32 ErrCode, UINT32 UsrSpecific)
{
    AppUsb_PrintUInt5("USB Device system error notification, ErrCode Num = %d, UsrSpecific = %d", ErrCode, UsrSpecific, 0, 0, 0);
}

static UINT32 usbd_cb_system_event(const UINT32 Code, const UINT32 Param1, const UINT32 Param2, const UINT32 Param3)
{
    static UINT32 connect_count = 0;

    switch (Code) {
    case USBD_SYSTEM_EVENT_CONNECT:
        AppUsb_PrintUInt5("Vbus Connect!! %d time(s)", (connect_count + 1U), 0, 0, 0, 0);
        connect_count++;
        break;
    case USBD_SYSTEM_EVENT_DISCONNECT:
        AppUsb_PrintUInt5("Vbus Disconnect!! %d time(s)", connect_count, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_CONFIGURED:
        AppUsb_PrintUInt5("Device System Configured, Index = %d", Param1, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_SUSPENDED:
        AppUsb_PrintUInt5("USB Device Suspended", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_RESUMED:
        AppUsb_PrintUInt5("USB Device Resumed", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_RESET:
        AppUsb_PrintUInt5("USB Device Reset", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_SET_FEATURED:
        AppUsb_PrintUInt5("USB Device Set Feature (%d, %d, 0x%X)", Param1, Param2, Param3, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_CLEAR_FEATURED:
        usbd_cb_clear_feature(Param1, Param2, Param3);
        break;
    case USBD_SYSTEM_EVENT_SET_INTERFACE:
        usbd_cb_set_intface(Param1, Param2);
        break;
    case USB_ERR_XHCI_EVT_BABBLE:
        AppUsb_Print("USB XHCI Event Ring BABBLE!");
        break;
    default:
        usbd_cb_error_notify(Code, Param1);
        break;
    }
    return 0;
}

static UINT32 usbd_cb_task_create(const UINT32 Code, USB_TASK_ENTRY_f TaskEntry, void *Arg, AMBA_KAL_TASK_t **pTask)
{
    UINT32 uret                       = 0;
    static char vbus_detection_task_name[] = "AmbaUSBD_RefVbusDetection";
    static char isr_task_name[]     = "AmbaUSBD_RefIsr";
    static char control_task_name[]    = "AmbaUSBD_RefCtrl";
    static UINT8 vbus_task_stack[VBUS_TASK_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 control_task_stack[CTRL_TASK_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 isr_task_stack[ISR_TASK_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static AMBA_KAL_TASK_t control_task;
    static AMBA_KAL_TASK_t ist_task;

    // Per Rio's request, no task creation in driver.
    // the Task is created by applications and we just provide entry function.
    switch (Code) {
    case USBD_VBUS_TASK_CODE:
        if (*pTask == NULL) {
            // new task creation
            uret = AppUsb_TaskCreate(&vbus_detection_task,
                                     vbus_detection_task_name,
                                     TASK_PRI_USBD_VBUS_DETECTION,
                                     TaskEntry,
                                     Arg,
                                     vbus_task_stack,
                                     VBUS_TASK_STACK_SIZE,
                                     1U, //AMBA_KAL_AUTO_START
                                     0x01);
            *pTask = &vbus_detection_task;
        } else {
            // resume task
            uret = AppUsb_TaskResume(*pTask);
        }

        {
            UINT32 address = 0;
            const AMBA_KAL_TASK_t *Src = *pTask;
            AmbaMisra_TypeCast32(&address,&Src);
            AppUsb_PrintUInt5("AppUsb_TaskCreate VBus detection: return 0x%x, Task 0x%X", uret, address, 0, 0, 0);
        }
        break;
    case USBD_CONTROL_REQUEST_TASK_CODE:
        if (*pTask == NULL) {
            uret = AppUsb_TaskCreate(&control_task,
                                     control_task_name,
                                     TASK_PRI_USBD_CONTROL_REQ,
                                     TaskEntry,
                                     Arg,
                                     control_task_stack,
                                     CTRL_TASK_STACK_SIZE,
                                     1U, //AMBA_KAL_AUTO_START
                                     0x01);
            *pTask = &control_task;
        } else {
            // resume task
            uret = AppUsb_TaskResume(*pTask);
        }

        {
            UINT32 address = 0;
            const AMBA_KAL_TASK_t *Src = *pTask;
            AmbaMisra_TypeCast32(&address,&Src);
            AppUsb_PrintUInt5("AppUsb_TaskCreate Ctrl: return 0x%x, Task 0x%X", uret, address, 0, 0, 0);
        }
        break;
    case USBD_ISR_TASK_CODE:
        if (*pTask == NULL) {
            uret = AppUsb_TaskCreate(&ist_task,
                                     isr_task_name,
                                     TASK_PRI_USBD_ISR_REQ,
                                     TaskEntry,
                                     Arg,
                                     isr_task_stack,
                                     ISR_TASK_STACK_SIZE,
                                     1U, //AMBA_KAL_AUTO_START
                                     0x01);
            *pTask = &ist_task;
        } else {
            // resume task
            uret = AppUsb_TaskResume(*pTask);
        }

        {
            UINT32 address = 0;
            const AMBA_KAL_TASK_t *Src = *pTask;
            AmbaMisra_TypeCast32(&address,&Src);
            AppUsb_PrintUInt5("AppUsb_TaskCreate Isr: return 0x%X, Task 0x%X", uret, address, 0, 0, 0);
        }
        break;
    default:
        uret = USB_ERR_FAIL;
        break;
    }
    return uret;
}
static UINT32 usbd_cb_task_suspend(const UINT32 Code, USB_TASK_ENTRY_f TaskEntry, AMBA_KAL_TASK_t *pTask)
{
    // Per Rio's request, no task deletion in driver.

    UINT32 uret = 0;
    (VOID)TaskEntry;

    switch (Code) {
    case 0:
        uret = AppUsb_TaskSuspend(pTask);
        break;
    case 1:
        uret = AppUsb_TaskSuspend(pTask);
        break;
    case 2:
        uret = AppUsb_TaskSuspend(pTask);
        break;
    default:
        uret = USB_ERR_FAIL;
        break;
    }

    {
        UINT32 address = 0;
        AmbaMisra_TypeCast32(&address,&pTask);
        AppUsb_PrintUInt5("AppUsb_TaskSuspend Task 0x%X return 0x%X for %d", address, uret, Code, 0, 0);
    }
    return uret;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Config GPIO pins for USB Host function
 * It configures OverCurrent and PowerEnable pins.
*/

void AppUsbh_GpioConfig(void)
{
#if !defined(CONFIG_DEVICE_TREE_SUPPORT)

    UINT32 i;
    UINT32 port_number;

#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2)
    static UINT32 host_port_power_pin[1] = {
        GPIO_PIN_37_USB0_EHCI_PWR,
    };

    static UINT32 host_port_oc_pin[1] = {
        GPIO_PIN_35_USB0_EHCI_OC,
    };
    port_number = 1;

#elif defined(CONFIG_SOC_CV25)
    static UINT32 host_port_power_pin[1] = {
        GPIO_PIN_26_USB0_EHCI_PWR,
    };

    static UINT32 host_port_oc_pin[1] = {
        GPIO_PIN_25_USB0_EHCI_OC,
    };
    port_number = 1;
#elif defined(CONFIG_SOC_H32)
    static UINT32 host_port_power_pin[1] = {
        GPIO_PIN_11_USB0_EHCI_PWR,
    };

    static UINT32 host_port_oc_pin[1] = {
        GPIO_PIN_10_USB0_EHCI_OC,
    };
    port_number = 1;
#if defined(CONFIG_BSP_H32BUB_OPTION_A_V300)
    AppUsb_Print("=============================================");
    AppUsb_Print("For H32BUB, please ensure:");
    AppUsb_Print("    1. 'diag console switch 1' has been typed to switch console");
    AppUsb_Print("    2. GPIO_SEL_6 switch to HIGH");
    AppUsb_Print("=============================================");
    AmbaPrint_Flush();
#endif
#elif defined(CONFIG_SOC_CV28)
    static UINT32 host_port_power_pin[1] = {
        GPIO_PIN_68_USB0_EHCI_PWR,
    };

    static UINT32 host_port_oc_pin[1] = {
        GPIO_PIN_67_USB0_EHCI_OC,
    };
    port_number = 1;
#elif defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    static UINT32 host_port_power_pin[1] = {
        GPIO_PIN_002_USB0_EHCI_PWR,
    };

    static UINT32 host_port_oc_pin[1] = {
        GPIO_PIN_001_USB0_EHCI_OC,
    };
    port_number = 1;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    static UINT32 host_port_power_pin[1] = {
        GPIO_PIN_128_USB32C_DRIVE_VBUS,
    };

    static UINT32 host_port_oc_pin[1] = {
        GPIO_PIN_127_USB32C_OVERCURRENT,
    };
    port_number = 1;
#else
#error "AppMsch_Start(): unknown chip version."
#endif

    // Configure GPIO
    for (i = 0; i < port_number; i++) {
        // Enable the EHCI over-current circuit.
        (VOID)AmbaGPIO_SetFuncAlt(host_port_power_pin[i]);
        // Config the GPIO as USB over current detection.
        (VOID)AmbaGPIO_SetFuncAlt(host_port_oc_pin[i]);
    }
#endif
}

/**
 * Stop USB Host system
*/
void AppUsbh_Stop(void)
{
    UINT32 uret = AmbaUSBH_SystemClassStop();

    AppUsb_PrintUInt5("AppUsbh_Stop, result = 0x%x", uret, 0, 0, 0, 0);
}
/**
 * Initialize USB device system
*/
UINT32 AppUsbd_SysInit(VOID)
{
    USBD_SYS_CONFIG_s sys_config;
    UINT32 uret;
    static UINT32 flag_init = 0;
    UINT32 module_id;
    UINT32 func_uret;
    const IOUT_MEMORY_INFO_s *noncache_mem_info;

    // enable USB/USBX print messages
    module_id = (USB_ERR_BASE >> 16U);
    func_uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, 1);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("Can't enable module printfor USB, code 0x%X", func_uret, 0, 0, 0, 0);
    }
    module_id = (USBX_ERR_BASE >> 16U);
    func_uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, 1);
    if (func_uret != 0U) {
        AppUsb_PrintUInt5("Can't enable module print for USBX, code 0x%X", func_uret, 0, 0, 0, 0);
    }

    if (usb_debug_level != 0xFFFFFFFFU) {
        AmbaUSB_SystemPrintFuncRegister(AppUsb_Print, usb_debug_level);
    }

    if (flag_init == 0U) {
        AppUsb_MemoryZeroSet(&sys_config, sizeof(USBD_SYS_CONFIG_s));
        AppUsb_MemoryZeroSet(&vbus_detection_task, sizeof(AMBA_KAL_TASK_t));
        flag_init = 1;
    }

    noncache_mem_info = AmbaIOUT_NoncacheMemInfoGet(IOUT_MEMID_USB);

    // USBX device won't use non-cache memory pool.
    // However CV5 USB device driver needs non-cache memory.
    sys_config.StackCacheMemory        = g_usbx_memory_cached;
    sys_config.StackCacheMemorySize    = USBX_CACHE_MEM_SIZE;
    sys_config.StackNonCacheMemory     = noncache_mem_info->Ptr;
    sys_config.StackNonCacheMemorySize = (UINT32)noncache_mem_info->Size;
    sys_config.CoreMask             = AMBA_KAL_CPU_CORE_MASK;
    sys_config.SystemEventCallback  = usbd_cb_system_event;
    sys_config.SystemTaskCreateCb   = usbd_cb_task_create;
    sys_config.SystemTaskSuspendCb  = usbd_cb_task_suspend;
    sys_config.InitSpeed            = USB_CONNECT_SPEED_HIGH;
    sys_config.SetCfgThd            = 1000;

    uret = AmbaUSBD_SystemInit(&sys_config);

    AppUsb_PrintUInt5("AmbaUSBD_System_Init: return 0x%x", uret, 0, 0, 0, 0);

    return uret;
}

/**
 * Register Device 'Clear Stall' event callback function
*/
UINT32 AppUsbd_ClearStallCBRegister(APPUSBD_CLR_STALL_CB CB)
{
    callback_clear_stall = CB;
    return USB_ERR_SUCCESS;
}
/**
 * Register Device 'Set Interface' event callback function
*/
UINT32 AppUsbd_SetInterfaceCBRegister(UINT32 InterfaceId, APPUSBD_SET_INTERFACE_CB CB)
{
    UINT32 uret;

    if (InterfaceId >= MAX_INTERFACE_CALLBACK) {
        uret = USB_ERR_FAIL;
    } else {
        callbacks_set_interface[InterfaceId] = CB;
        uret                                 = USB_ERR_SUCCESS;
    }

    return uret;
}
/**
 * Start USB device class
*/
VOID AppUsbd_ClassStart(VOID)
{
    USBD_CLASS_CONFIG_s ClassConfig = {
        USBD_CLASS_NONE,
        NULL,
        NULL,
        NULL
    };

    UINT32 uret;

    if (usbd_current_class_id == APPUSBD_CLASS_MSC) {
        ClassConfig.ClassID   = USBD_CLASS_MSC;
        ClassConfig.Framework = &AppUsbd_MscDescFramework;
        ClassConfig.ClassStartCb = AmbaUSBD_MSCClassStart;
        ClassConfig.ClassStopCb  = AmbaUSBD_MSCClassStop;
    } else if (usbd_current_class_id == APPUSBD_CLASS_MTP) {
        ClassConfig.ClassID   = USBD_CLASS_MTP;
        ClassConfig.Framework = &g_mtp_desc_framework;
        ClassConfig.ClassStartCb = AmbaUSBD_MTPClassStart;
        ClassConfig.ClassStopCb  = AmbaUSBD_MTPClassStop;
    } else if (usbd_current_class_id == APPUSBD_CLASS_CDC_ACM) {
        ClassConfig.ClassID   = USBD_CLASS_CDC_ACM;
        ClassConfig.Framework = &AppUsbd_CdcAcmDescFramework;
        ClassConfig.ClassStartCb = AmbaUSBD_CDCACMClassStart;
        ClassConfig.ClassStopCb  = AmbaUSBD_CDCACMClassStop;
    } else if (usbd_current_class_id == APPUSBD_CLASS_UVC) {
        ClassConfig.ClassID   = USBD_CLASS_USER;
        ClassConfig.Framework = &g_uvc_desc_framework;
        ClassConfig.ClassStartCb = AmbaUSBD_UVCClassStart;
        ClassConfig.ClassStopCb  = AmbaUSBD_UVCClassStop;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (usbd_current_class_id == APPUSBD_CLASS_CDC_MSC) {
        ClassConfig.ClassID   = USBD_CLASS_USER;
        ClassConfig.Framework = &g_cdc_msc_desc_framework;
        ClassConfig.ClassStartCb = AppCdcMscd_ClassStart;
        ClassConfig.ClassStopCb  = AppCdcMscd_ClassStop;
    } else if (usbd_current_class_id == APPUSBD_CLASS_CDC_MTP) {
        ClassConfig.ClassID   = USBD_CLASS_USER;
        ClassConfig.Framework = &g_cdc_mtp_desc_framework;
        ClassConfig.ClassStartCb = AppCdcMtpd_ClassStart;
        ClassConfig.ClassStopCb  = AppCdcMtpd_ClassStop;
    } else if (usbd_current_class_id == APPUSBD_CLASS_CDC_ACM_MULTI) {
        ClassConfig.ClassID   = USBD_CLASS_USER;
        ClassConfig.Framework = &g_cdc_acm_multi_desc_framework;
        ClassConfig.ClassStartCb = AppCdcAcmMultid_ClassStart;
        ClassConfig.ClassStopCb  = AppCdcAcmMultid_ClassStop;
#endif
    } else {
        AppUsb_PrintUInt5("Unknown class", 0, 0, 0, 0, 0);
    }

    uret = AmbaUSBD_SystemClassStart(&ClassConfig);
    AppUsb_PrintUInt5("Class start result = 0x%x", uret, 0, 0, 0, 0);
}
/**
 * Stop USB device class
*/
VOID AppUsbd_ClassStop(VOID)
{
    UINT32 uret = AmbaUSBD_SystemClassStop();
    AppUsb_PrintUInt5("Class stop result = 0x%x", uret, 0, 0, 0, 0);
}
/**
 * Force to release USB device system
*/
VOID AppUsbd_BruteRelease(VOID)
{
    UINT32 uret;

    uret = AmbaUSBD_SystemDeInit();
    AppUsb_PrintUInt5("Brute release result = 0x%x", uret, 0, 0, 0, 0);
}
/**
 * Print USB device charger detection status
*/
void AppUsbd_ChargerDet(UINT32 T1, UINT32 T2)
{
    UINT32 charger_type = 0;
    UINT32 uret;

    uret = AmbaUSBD_SystemChargerDetection(T1, T2, &charger_type);

    if (uret == USB_ERR_SUCCESS) {
        if (charger_type == (UINT32)USBD_CHGDET_SDP) {
            AppUsb_Print("AmbaUSBD_System_ChargerDetection: It's a standard downstream port");
        } else if (charger_type == (UINT32)USBD_CHGDET_DCP) {
            AppUsb_Print("AmbaUSBD_System_ChargerDetection: It's a dedicate charger port");
        } else if (charger_type == (UINT32)USBD_CHGDET_CDP) {
            AppUsb_Print("AmbaUSBD_System_ChargerDetection: It's a charger downstream port");
        } else if (charger_type == (UINT32)USBD_CHGDET_NO_CONTACT) {
            AppUsb_Print("AmbaUSBD_System_ChargerDetection: no data contact");
        } else {
            // shall not be here.
        }
    } else {
        if (uret == USB_ERR_NO_INSTANCE) {
            AppUsb_Print("AmbaUSBD_System_ChargerDetection: hardware not support.");
        } else {
            AppUsb_PrintUInt5("AmbaUSBD_System_ChargerDetection: Fail, err = 0x%x", uret, 0, 0, 0, 0);
        }
    }
}
/**
 * Print USB device VBUS status
*/
void AppUsbd_VbusStatusPrint(void)
{
    UINT32 status = 0;
    UINT32 uret   = AmbaUSBD_SystemGetVbusStatus(&status);

    if (uret == USB_ERR_SUCCESS) {
        AppUsb_PrintUInt5("Vbus Status = %d", status, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AppUsbd_VbusStatusPrint() failed, code 0x%X", uret, 0, 0, 0, 0);
    }
}
/**
 * Print USB device enumeration speed
*/
void AppUsbd_EnumSpeedPrint(void)
{
    UINT32 speed = 0;
    UINT32 uret  = AmbaUSBD_SystemGetConnectSpeed(&speed);

    if (uret == USB_ERR_SUCCESS) {
        switch(speed) {
        case USB_CONNECT_SPEED_FULL:
            AppUsb_Print("FULL SPEED(11Mb)");
            break;
        case USB_CONNECT_SPEED_HIGH:
            AppUsb_Print("HIGH SPEED(480Mb)");
            break;
        case USB_CONNECT_SPEED_SUPER:
            AppUsb_Print("SUPER SPEED (5Gb)");
            break;
        case USB_CONNECT_SPEED_SUPER_PLUS:
            AppUsb_Print("SUPER SPEED PLUS (10Gb)");
            break;
        default:
            AppUsb_Print("UNKNOWN SPEED");
            break;
        }
    } else {
        AppUsb_PrintUInt5("AmbaUSBD_SystemGetConnectSpeed() failed, code 0x%X", uret, 0, 0, 0, 0);
    }
}
/**
 * Print if USB device is configured by Host
*/
void AppUsbd_IsConfiguredPrint(void)
{
    UINT32 is_configured = 0;
    UINT32 uret          = AmbaUSBD_SystemIsConfigured(&is_configured);

    if (uret == USB_ERR_SUCCESS) {
        AppUsb_PrintUInt5("Configure = %d", is_configured, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AmbaUSBD_SystemIsConfigured() failed, code 0x%X", uret, 0, 0, 0, 0);
    }
}

/**
 * Print USB system version information
*/
void AppUsbd_VersionPrint(void)
{
    UINT32 uret;
    USB_VER_INFO_s ver;

    uret = AmbaUSB_SystemGetVersionInfo(&ver);

    if (uret == USB_ERR_SUCCESS) {
        AppUsb_PrintUInt5("Version: %X.%X", ver.MajorVersion, ver.MinorVersion, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("AmbaUSB_SystemGetVersionInfo() failed, code 0x%X", uret, 0, 0, 0, 0);
    }
}

/**
 * Set current USB Device class ID
*/
VOID AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_e ClassID)
{
    usbd_current_class_id = ClassID;
}

APPUSBD_CLASS_e AppUsbd_CurrentClassIDGet(void)
{
    return usbd_current_class_id;
}

void AppUsbh_TestModeSet(UINT32 TestMode)
{
    UINT32 uret = AmbaUSBH_SystemSetTestMode(TestMode);
    if (uret != 0U) {
        AppUsb_PrintUInt5("AmbaUSBH_SystemSetTestMode() failed, code 0x%X", uret, 0, 0, 0, 0);
    }
}

/** @} */
