/**
 *  @file AmbaUSBH_System.c
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
 *  @details USB kernel driver phy function.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <usbx/ux_host_stack.h>
#include <usbx/usbx_host_controllers/ux_hcd_ehci.h>
#include <usbx/usbx_host_controllers/ux_hcd_ohci.h>
#ifndef AMBA_MISRA_FIX_H
#include <AmbaMisraFix.h>
#endif
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaGPIO.h>
#endif

#define USBH_FLAG_FLOW_HOST_INIT   ((UINT32)0x80000000UL)
#define USBH_FLAG_FLOW_USBX_INIT   ((UINT32)0x00000001UL)
#define USBH_FLAG_FLOW_HW_INIT     ((UINT32)0x00000002UL)
#define USBH_FLAG_FLOW_CLASS_INIT  ((UINT32)0x00000004UL)
#define USBH_FLAG_FLOW_HCD_INIT    ((UINT32)0x00000008UL)

#define USBH_MAXIMUM_SUPPORT_CLASS 2U

typedef struct {
    UINT32 ClassNum;
    UINT8 *ClassName[USBH_MAXIMUM_SUPPORT_CLASS];
    USBH_DEVICE_CHANGE_f ChangeFunc[USBH_MAXIMUM_SUPPORT_CLASS];
} HOST_DEVICE_CHANGE_INFO_s;

typedef struct {
    UX_HOST_CLASS *UxHostClass;
    const void    *UxInstance;
} USBX_CONTEXT;

static UINT32 flag_init = 0;
static USBH_CLASS_CONFIG_s usbh_class_config USB_ATTR_NO_INIT;
static HOST_DEVICE_CHANGE_INFO_s device_change_info USB_ATTR_NO_INIT;
static UINT8   *CtrlrMemPtr  = NULL;
static UINT32   CtrlrMemSize = 0;

static UCHAR host_hcd_ehci_name[] =  "ux_hcd_ehci";

static void initflag_set(UINT32 Flag)
{
    flag_init |= Flag;
}

static UINT32 initflag_check(UINT32 Flag)
{
    UINT32 uret;

    if ((flag_init & Flag) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}

static void initflag_clear(UINT32 Flag)
{
    flag_init &= (~Flag);
}

static UINT32 usbh_check_device_mode(void)
{
    return USBD_SystemIsInit();
}

static UINT32 usbh_check_and_store_parameter(const USBH_CLASS_CONFIG_s *Config)
{
    UINT32 uret;

    if (Config == NULL) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((Config->StackCacheMemory == NULL) ||
               (Config->StackNonCacheMemory == NULL) ||
               (Config->SystemEventCallback == NULL) ||
               (Config->ClassStartCallback == NULL) ||
               (Config->ClassStopCallback == NULL)
              ) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if (Config->CoreMask == 0U) {
        uret = USB_ERR_COREMASK_ZERO;
    } else if ((Config->SelectHcd > USBH_HCD_XHCI) ||
               (Config->EhciOcPolarity > 1U) ||
               (Config->Phy0Owner > 1U) ||
               (Config->StackCacheMemorySize < (15UL * 1024UL)) ||
               (Config->StackNonCacheMemorySize == 0U) ||
               (Config->EnumTskInfo.StackSize == 0U) ||
               (Config->HcdTskInfo.StackSize == 0U)) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if ((Config->ClassID != USBH_CLASS_STORAGE) &&
               (Config->ClassID != USBH_CLASS_SIMPLE) &&
               (Config->ClassID != USBH_CLASS_CUSTOM)) {
        uret = USB_ERR_PARAMETER_INVALID;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (Config->SelectHcd != USBH_HCD_XHCI) {
        uret = USB_ERR_PARAMETER_INVALID;
#endif
    } else {
        uret = USB_ERR_SUCCESS;
        if (usbh_check_device_mode() == 1U) {
            uret = USB_ERR_DEVICE_IS_RUNNING;
        }

        if (uret == USB_ERR_SUCCESS) {

            UINT32 controller_mem_size = USBCTRLR_MEMORY_SIZE;

            USB_UtilityMemoryCopy(&usbh_class_config, Config, sizeof(USBH_CLASS_CONFIG_s));

            CtrlrMemPtr  = NULL;
            CtrlrMemSize = 0;

            if (usbh_class_config.StackNonCacheMemorySize > controller_mem_size) {
                CtrlrMemPtr  = usbh_class_config.StackNonCacheMemory;
                CtrlrMemSize = controller_mem_size;

                // twick non-cache memory size
                usbh_class_config.StackNonCacheMemorySize -= controller_mem_size;
                usbh_class_config.StackNonCacheMemory = &usbh_class_config.StackNonCacheMemory[controller_mem_size];
            }
        }
    }

    return uret;
}

static UINT usbh_device_change_func(ULONG Operation, UX_HOST_CLASS *HostClass, const void *Instance)
{
    UINT32 uret = 0, i;
    UX_HOST_CLASS *tmp_host_class;
    USBX_CONTEXT   usbx_ctx;
    const USBX_CONTEXT  *usbx_context_ptr = &usbx_ctx;
    UINT32         sub_code;

    usbx_ctx.UxHostClass = HostClass;
    usbx_ctx.UxInstance  = Instance;

    AmbaMisra_TypeCast32(&sub_code, &usbx_context_ptr);

    // HostClass is not used for MSC class.
    // For customized class implementation, HostClass and Instance should be taken from USBX directly.
    (void)HostClass;

    for (i = 0; i < USBH_MAXIMUM_SUPPORT_CLASS; i++) {
        if (device_change_info.ClassName[i] != NULL) {
            uret = ux_host_stack_class_get(device_change_info.ClassName[i], &tmp_host_class);
            if (uret == (UINT32)UX_SUCCESS) {
                if (tmp_host_class == HostClass) {
                    if (HostClass != NULL) {
                        UINT32 class_status = HostClass->ux_host_class_status;

                        // fix the vcast check.
                        HostClass->ux_host_class_status = class_status;
                    }
                    uret = device_change_info.ChangeFunc[i](Operation, Instance);
                    break;
                }
            }
        }
    }

    if (usbh_class_config.SystemEventCallback != NULL) {
        switch (Operation) {
        case UX_DEVICE_INSERTION:
            if (usbh_class_config.SystemEventCallback(USBH_EVENT_DEVICE_INSERTED, sub_code) != 0U) {
                // ignore error
            }
            break;
        case UX_DEVICE_REMOVAL:
            if (usbh_class_config.SystemEventCallback(USBH_EVENT_DEVICE_REMOVED, sub_code) != 0U) {
                // ignore error
            }
            break;
        default:
            // unsupported operation
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "usbh_device_change_func: Unsupport operaion = 0x%x", (UINT32)Operation, 0, 0, 0, 0);
            break;
        }
    }

    return uret;
}

UINT32 USBH_DeviceChangeFuncHook(UINT8* pClassName, USBH_DEVICE_CHANGE_f ChangeFunc)
{
    UINT32 uret;
    UINT32 i;

    if ((pClassName == NULL) || (ChangeFunc == NULL)) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if (device_change_info.ClassNum == USBH_MAXIMUM_SUPPORT_CLASS) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "AmbaUSBH_System_HookDeviceChangeFunc: reach class number limit");
        uret = USB_ERR_REACH_MAX_NUMBER;
    } else {
        for (i = 0; i < USBH_MAXIMUM_SUPPORT_CLASS; i++) {
            if (device_change_info.ClassName[i] == NULL) {
                device_change_info.ClassName[i]  = pClassName;
                device_change_info.ChangeFunc[i] = ChangeFunc;
                device_change_info.ClassNum++;
            }
        }
        uret = USB_ERR_SUCCESS;
    }

    return uret;
}

UINT32 USBH_DeviceChangeFuncUnHook(UINT8* pClassName)
{
    UINT32 i;
    UINT32 uRet = USB_ERR_SUCCESS;

    if (pClassName == NULL) {
        uRet = USB_ERR_PARAMETER_NULL;
    } else if (device_change_info.ClassNum == 0U) {
        // print debug message
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "AmbaUSBH_System_UnHookDeviceChangeFunc: hooked class number is zero");
    } else {
        for (i = 0; i < USBH_MAXIMUM_SUPPORT_CLASS; i++) {
            if (device_change_info.ClassName[i] != NULL) {
                if (_ux_utility_memory_compare(device_change_info.ClassName[i], pClassName, _ux_utility_string_length_get(pClassName)) == (UINT32)UX_SUCCESS) {
                    device_change_info.ClassName[i]  = NULL;
                    device_change_info.ChangeFunc[i] = NULL;
                    device_change_info.ClassNum--;
                }
            }
        }
    }
    return uRet;
}

static void device_no_respond_notify(void)
{
    if (usbh_class_config.SystemEventCallback != NULL) {
        UINT32 uret = usbh_class_config.SystemEventCallback(USBH_EVENT_DEVICE_NO_RESPONSE, 0);
        if (uret != USB_ERR_SUCCESS) {
            // ignore error
        }
    }
}
static void device_not_support_notify(void)
{
    UINT32 uret = usbh_class_config.SystemEventCallback(USBH_EVENT_DEVICE_NOT_SUPPORT, 0);

    if (uret != USB_ERR_SUCCESS) {
        // ignore error
    }
}
static void device_over_current_notify(void)
{
    UINT32 uret = usbh_class_config.SystemEventCallback(USBH_EVENT_DEVICE_OVER_CURRENT, 0);

    if (uret != USB_ERR_SUCCESS) {
        // ignore error
    }
}
static void hub_not_support_notify(void)
{
    UINT32 uret = usbh_class_config.SystemEventCallback(USBH_EVENT_HUB_NOT_SUPPORT, 0);

    if (uret != USB_ERR_SUCCESS) {
        // ignore error
    }
}

UINT32 USBH_SystemCtrlrMemGet(UINT8 **Ptr, UINT32 *Size)
{
    UINT32 uret;

    if (CtrlrMemPtr == NULL) {
        uret  = USB_ERR_NO_INIT;
        *Ptr  = NULL;
        *Size = 0;
    } else {
        uret    = USB_ERR_SUCCESS;
        *Ptr    = CtrlrMemPtr;
        *Size   = CtrlrMemSize;
    }

    return uret;
}

static UINT32 usbh_init_usbx(const USBH_CLASS_CONFIG_s *Config)
{
    UINT32 uret;
    static UX_HOST_ERR_NOFIFY_CB error_notify_callbacks = {
        device_no_respond_notify,
        device_not_support_notify,
        device_over_current_notify,
        hub_not_support_notify
    };

    // Setup the parameters.
    uhc_virt_to_phy_func_set(USB_UtilityVirVp2PhyVp);
    uhc_phy_to_virt_func_set(USB_UtilityPhyVp2VirVp);
    uhc_tsk_enum_stk_size_set(Config->EnumTskInfo.StackSize);
    uhc_tsk_hcd_stk_size_set(Config->HcdTskInfo.StackSize);
    uhc_tsk_enum_pri_set(Config->EnumTskInfo.Priority);
    uhc_tsk_hcd_pri_set(Config->HcdTskInfo.Priority);
    (void)uhc_tsk_enum_afmask_set(Config->EnumTskInfo.AffinityMask);
    (void)uhc_tsk_hcd_afmask_set(Config->HcdTskInfo.AffinityMask);
    uhc_notify_callback_set(&error_notify_callbacks);

    // Setup USBX system memory
    uret = ux_system_initialize(Config->StackCacheMemory,
                                Config->StackCacheMemorySize,
                                Config->StackNonCacheMemory,
                                Config->StackNonCacheMemorySize);

    if (uret != (UINT32)UX_SUCCESS) {
        uret = USBX_ERR_SYSTEM_INIT_FAIL;
    }

    if (uret == (UINT32)UX_SUCCESS) {
        uret = ux_host_stack_initialize(usbh_device_change_func);
        if (uret != (UINT32)UX_SUCCESS) {
            // Shall not be here when static memory is used.
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "ux_host_stack_initialize fail, status = 0x%x", uret, 0, 0, 0, 0);
            if (uret == (UINT32)UX_MEMORY_INSUFFICIENT) {
                uret = USBX_ERR_HSTACK_INIT_NO_MEMORY;
            } else if (uret == (UINT32)UX_SEMAPHORE_ERROR) {
                uret = USBX_ERR_HSTACK_INIT_SEMAPHORE_ERROR;
            } else if (uret == (UINT32)UX_THREAD_ERROR) {
                uret = USBX_ERR_HSTACK_INIT_THREAD_ERROR;
            } else {
                uret = USBX_ERR_HSTACK_INIT_FAIL;
            }
        }
    }

    return uret;
}

static UINT32 usbh_deinit_usbx(void)
{
    UINT32 func_uret;
    UINT32 uret = USB_ERR_SUCCESS;

    func_uret = ux_host_stack_destroy();
    if (func_uret != (UINT32)UX_SUCCESS) {
        if (func_uret == (UINT32)UX_THREAD_ERROR) {
            func_uret = USBX_ERR_HSTACK_DEINIT_THREAD_ERROR;
        } else if (func_uret == (UINT32)UX_SEMAPHORE_ERROR) {
            func_uret = USBX_ERR_HSTACK_DEINIT_SEMAPHORE_ERROR;
        } else {
            func_uret = USBX_ERR_HSTACK_DEINIT_FAIL;
        }
        uret = func_uret;
    }

    func_uret = ux_system_destroy();
    if (func_uret != (UINT32)UX_SUCCESS) {
        func_uret = USBX_ERR_SYSTEM_DEINIT_FAIL;
        if (uret == USB_ERR_SUCCESS) {
            // only assign error code if previous API successes.
            uret = func_uret;
        }
    }

    return uret;
}

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
static void host_gpio_pins_init(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    if (hws->HostPinOverCurrent != 0U) {
        if (AmbaGPIO_SetFuncAlt(hws->HostPinOverCurrent) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USB] can't set Host OverCurrent pin 0x%X", hws->HostPinOverCurrent, 0, 0, 0, 0);
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "[USB] set Host OverCurrent pin 0x%X", hws->HostPinOverCurrent, 0, 0, 0, 0);
        }
    }

    if (hws->HostPinPowerEnable != 0U) {
        if (AmbaGPIO_SetFuncAlt(hws->HostPinPowerEnable) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USB] can't set Host Power Enable pin 0x%X", hws->HostPinPowerEnable, 0, 0, 0, 0);
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "[USB] set Host Power Enable pin 0x%X", hws->HostPinPowerEnable, 0, 0, 0, 0);
        }
    }
}
#endif

static void hardware_init(const USBH_CLASS_CONFIG_s *Config)
{
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    host_gpio_pins_init();
#endif

    USBH_UhcInit(Config);
}

static void hardware_deinit(void)
{
    USBH_UhcDeInit();
}

static UINT32 usbh_class_init(const USBH_CLASS_CONFIG_s *Config)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (Config->ClassID == USBH_CLASS_STORAGE) {
        uret = Config->ClassStartCallback(USBH_CLASS_STORAGE);
    } else if (Config->ClassID == USBH_CLASS_SIMPLE) {
        uret = Config->ClassStartCallback(USBH_CLASS_SIMPLE);
    } else if (Config->ClassID == USBH_CLASS_CUSTOM) {
        uret = Config->ClassStartCallback(USBH_CLASS_CUSTOM);
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Class ID %d is not supported", (UINT32)Config->ClassID, 0, 0, 0, 0);
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}

static UINT32 usbh_class_deinit(const USBH_CLASS_CONFIG_s *Config)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (Config->ClassID == USBH_CLASS_STORAGE) {
        uret = Config->ClassStopCallback(USBH_CLASS_STORAGE);
    } else if (Config->ClassID == USBH_CLASS_SIMPLE) {
        uret = Config->ClassStopCallback(USBH_CLASS_SIMPLE);
    } else if (Config->ClassID == USBH_CLASS_CUSTOM) {
        uret = Config->ClassStopCallback(USBH_CLASS_CUSTOM);
    } else {
        // print debug message
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Class ID %d is not supported", (UINT32)Config->ClassID, 0, 0, 0, 0);
    }
    return uret;
}

static UINT32 ux_ohci_code_to_amba_code(UINT32 Code)
{
    UINT32 uret = USBX_ERR_FAIL;

    if (Code == (UINT32)UX_MEMORY_INSUFFICIENT) {
        uret = USBX_ERR_OHCI_INIT_NO_MEMORY;
    } else if (Code == (UINT32)UX_CONTROLLER_INIT_FAILED) {
        uret = USBX_ERR_OHCI_INIT_CONTROLLER_FAIL;
    } else if (Code == (UINT32)UX_NO_ED_AVAILABLE) {
        uret = USBX_ERR_OHCI_INIT_NO_AVAILABLE_ED;
    } else {
        uret = USBX_ERR_OHCI_INIT_FAIL;
    }
    return uret;
}

static UINT32 ux_ehci_code_to_amba_code(UINT32 Code)
{
    UINT32 uret = USBX_ERR_FAIL;

    if (Code == (UINT32)UX_MEMORY_INSUFFICIENT) {
        uret = USBX_ERR_EHCI_INIT_NO_MEMORY;
    } else if (Code == (UINT32)UX_NO_ED_AVAILABLE) {
        uret = USBX_ERR_EHCI_INIT_NO_AVAILABLE_ED;
    } else {
        uret = USBX_ERR_EHCI_INIT_FAIL;
    }
    return uret;
}


static UINT32 usbh_hcd_init(const USBH_CLASS_CONFIG_s *Config)
{
    static UCHAR host_hcd_ohci_name[] =  "ux_hcd_ohci";
    UINT32 uret;
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();

    // We expect the HCD would start after the HCD initialization.
    if (Config->SelectHcd == USBH_HCD_OHCI) {
        uhc_ohci_enabled_set(1);
        uret = ux_host_stack_hcd_register(host_hcd_ohci_name, _ux_hcd_ohci_initialize, (ULONG)hws->Ohci.BaseAddress, 0x0);
        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[MSCH] can't init OHCI, status = 0x%x", uret, 0, 0, 0, 0);
            uret = ux_ohci_code_to_amba_code(uret);
        }
    } else if (Config->SelectHcd == USBH_HCD_EHCI) {
        uret = ux_host_stack_hcd_register(host_hcd_ehci_name, _ux_hcd_ehci_initialize, (ULONG)hws->Ehci.BaseAddress, 0x0);
        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "_ux_hcd_ehci_initialize fail, status = 0x%x", uret, 0, 0, 0, 0);
            uret = ux_ehci_code_to_amba_code(uret);
        }
    } else if (Config->SelectHcd == USBH_HCD_EHCI_OHCI) {
        uhc_ohci_enabled_set(1);
        uret = ux_host_stack_hcd_register(host_hcd_ehci_name, _ux_hcd_ehci_initialize, (ULONG)hws->Ehci.BaseAddress, 0x0);
        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "_ux_hcd_ehci_initialize fail, status = 0x%x", uret, 0, 0, 0, 0);
            uret = ux_ehci_code_to_amba_code(uret);
        } else {
            uret = ux_host_stack_hcd_register(host_hcd_ohci_name, _ux_hcd_ohci_initialize, (ULONG)hws->Ohci.BaseAddress, 0x0);
            if (uret != (UINT32)UX_SUCCESS) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[MSCH] can't init OHCI, status = 0x%x", uret, 0, 0, 0, 0);
                uret = ux_ohci_code_to_amba_code(uret);
            }
        }
#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)) && defined(CONFIG_BUILD_CADENCE_USB3_DRIVER)
    } else if (Config->SelectHcd == USBH_HCD_XHCI) {
        static UCHAR host_hcd_xhci_name[] =  "ux_hcd_xhci";
        uret = ux_host_stack_hcd_register(host_hcd_xhci_name, ux_hcd_xhci_initialize, (ULONG)hws->Xhci.BaseAddress, 0x0);
        if (uret != (UINT32)UX_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_initialize fail, status = 0x%x", uret, 0, 0, 0, 0);
        }
#endif
    } else {
        // shall not be here.
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}

static UINT32 usbh_hcd_stop(const USBH_CLASS_CONFIG_s *Config)
{
    UINT32 uret;
    UINT32 ehci_status;

    // We expect the HCD would start after the HCD initialization.
    if (Config->SelectHcd == USBH_HCD_OHCI) {
        uret = _ux_hcd_ohci_stop();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_OHCI_STOP_NO_HCD_REGISTERED;
            } else {
                uret = USBX_ERR_OHCI_STOP_FAIL;
            }
        }
    } else if (Config->SelectHcd == USBH_HCD_EHCI) {
        uret = _ux_hcd_ehci_stop();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_EHCI_STOP_NO_HCD_REGISTERED;
            } else {
                uret = USBX_ERR_EHCI_STOP_FAIL;
            }
        }
    } else if (Config->SelectHcd == USBH_HCD_EHCI_OHCI) {
        uret = _ux_hcd_ohci_stop();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_OHCI_STOP_NO_HCD_REGISTERED;
            } else {
                uret = USBX_ERR_OHCI_STOP_FAIL;
            }
        }
        ehci_status = _ux_hcd_ehci_stop();
        if (ehci_status != (UINT32)UX_SUCCESS) {
            if (ehci_status == 0xFFFFFFFFU) {
                uret = USBX_ERR_EHCI_STOP_NO_HCD_REGISTERED;
            } else {
                uret = USBX_ERR_EHCI_STOP_FAIL;
            }
        }

#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)) && defined(CONFIG_BUILD_CADENCE_USB3_DRIVER)
    } else if (Config->SelectHcd == USBH_HCD_XHCI) {
        uret = ux_hcd_xhci_stop();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_XHCI_STOP_NO_HCD_REGISTERED;
            } else {
                uret = USBX_ERR_XHCI_STOP_FAIL;
            }
        }
#endif
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    // DO NOT remove all existed device resources before USBX stack is released
    // it may be called twice if enumeration fail in USBX stack.

    return uret;
}

static UINT32 usbh_hcd_deinit(const USBH_CLASS_CONFIG_s *Config)
{
    UINT32 uret = USB_ERR_SUCCESS;

    // We expect the HCD would start after the HCD initialization.
    if (Config->SelectHcd == USBH_HCD_OHCI) {
        uret = _ux_hcd_ohci_destroy();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_OHCI_DEINIT_NO_HCD_REGISTERED;
            } else {
                uret = USBX_ERR_OHCI_DEINIT_FAIL;
            }
        }
    } else if (Config->SelectHcd == USBH_HCD_EHCI) {
        uret = _ux_hcd_ehci_destroy();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_EHCI_DEINIT_NO_HCD_REGISTERED;
            } else if (uret == (UINT32)UX_SEMAPHORE_ERROR) {
                uret = USBX_ERR_EHCI_DEINIT_SEMAPHORE_ERROR;
            } else {
                uret = USBX_ERR_EHCI_DEINIT_FAIL;
            }
        }
    } else if (Config->SelectHcd == USBH_HCD_EHCI_OHCI) {
        uret = _ux_hcd_ohci_destroy();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_OHCI_DEINIT_NO_HCD_REGISTERED;
            } else {
                uret = USBX_ERR_OHCI_DEINIT_FAIL;
            }
        } else {
            uret = _ux_hcd_ehci_destroy();
            if (uret != (UINT32)UX_SUCCESS) {
                if (uret == 0xFFFFFFFFU) {
                    uret = USBX_ERR_EHCI_DEINIT_NO_HCD_REGISTERED;
                } else if (uret == (UINT32)UX_SEMAPHORE_ERROR) {
                    uret = USBX_ERR_EHCI_DEINIT_SEMAPHORE_ERROR;
                } else {
                    uret = USBX_ERR_EHCI_DEINIT_FAIL;
                }
            }
        }
#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)) && defined(CONFIG_BUILD_CADENCE_USB3_DRIVER)
    } else if (Config->SelectHcd == USBH_HCD_XHCI) {
        uret = ux_hcd_xhci_destroy();
        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == 0xFFFFFFFFU) {
                uret = USBX_ERR_XHCI_DEINIT_NO_HCD_REGISTERED;
            } else if (uret == (UINT32)UX_SEMAPHORE_ERROR) {
                uret = USBX_ERR_XHCI_DEINIT_SEMAPHORE_ERROR;
            } else {
                uret = USBX_ERR_XHCI_DEINIT_FAIL;
            }
        }
#endif
    } else {
        // shall not be here.
        uret = USB_ERR_PARAMETER_INVALID;
    }

    // Remove all existed device resource after USBX stack is released
    // Don't check the return value since it would returns UX_DEVICE_HANDLE_UNKNOWN
    // in normal condition.
    (void)_ux_host_stack_device_remove_all();

    return uret;
}
/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * called by driver to check if system is initialized.
*/
UINT32 USBH_SystemIsInit(void)
{
    UINT32 uret;

    if (initflag_check(USBH_FLAG_FLOW_HOST_INIT) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}
/**
 * called by driver to check if USBX system is initialized.
*/
UINT32 USBH_SystemIsUSBXInited(void)
{
    UINT32 uret;

    if (initflag_check(USBH_FLAG_FLOW_USBX_INIT) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}

/** @} */

/** \addtogroup Public_APIs
 *  @{
 * */
/**
 * This function is used for starting the USBX host controller and class client software.
 * @param Config [Input] Define USB host class Initial parameters. Note that the priority of the class thread should not be higher than the host controller and enumeration.
*/
UINT32 AmbaUSBH_SystemClassStart(const USBH_CLASS_CONFIG_s *Config)
{
    UINT32 uret;

    if (initflag_check(USBH_FLAG_FLOW_HOST_INIT) == 0U) {
        uret = usbh_check_and_store_parameter(Config);
        if (uret == USB_ERR_SUCCESS) {
            // initialize RTSL
            USB_UtilityRTSLInit();

            // Initialize the Utility.
            USB_UtilityCoreMaskSet(usbh_class_config.CoreMask);

            // Init parameter.
            USB_UtilityMemorySet(&device_change_info, 0, sizeof(HOST_DEVICE_CHANGE_INFO_s));

            if (initflag_check(USBH_FLAG_FLOW_USBX_INIT) == 0U) {
                uret = usbh_init_usbx(&usbh_class_config);
                initflag_set(USBH_FLAG_FLOW_USBX_INIT);
            } else {
                // print debug message
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "usbx has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
            }

            if (uret == USB_ERR_SUCCESS) {
                if (initflag_check(USBH_FLAG_FLOW_HW_INIT) == 0U) {
                    hardware_init(&usbh_class_config);
                    initflag_set(USBH_FLAG_FLOW_HW_INIT);
                } else {
                    // print debug message
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host hw has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
                }
            }

            if (uret == USB_ERR_SUCCESS) {
                if (initflag_check(USBH_FLAG_FLOW_CLASS_INIT) == 0U) {
                    uret = usbh_class_init(&usbh_class_config);
                    initflag_set(USBH_FLAG_FLOW_CLASS_INIT);
                } else {
                    // print debug message
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host class has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
                }
            }

            if (uret == USB_ERR_SUCCESS) {
                if (initflag_check(USBH_FLAG_FLOW_HCD_INIT) == 0U) {
                    uret = usbh_hcd_init(&usbh_class_config);
                    initflag_set(USBH_FLAG_FLOW_HCD_INIT);
                } else {
                    // print debug message
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "hcd has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
                }
            }

            initflag_set(USBH_FLAG_FLOW_HOST_INIT);

        }
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "AmbaUSBH_System_ClassStart is called before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        uret = USB_ERR_SUCCESS;
    }

    return uret;
}

/**
 * This function is used for release of the USBX host class client software.
*/
UINT32 AmbaUSBH_SystemClassStop(void)
{
    UINT32 fucn_uret = USB_ERR_SUCCESS;
    UINT32 uret= USB_ERR_SUCCESS;

    if (initflag_check(USBH_FLAG_FLOW_HOST_INIT) != 0U) {
        // Stop the controller and remove the attached device.
        fucn_uret = usbh_hcd_stop(&usbh_class_config);

        if (fucn_uret != USB_ERR_SUCCESS) {
            uret = fucn_uret;
        }

        if (initflag_check(USBH_FLAG_FLOW_CLASS_INIT) != 0U) {
            fucn_uret = usbh_class_deinit(&usbh_class_config);
            if (fucn_uret != USB_ERR_SUCCESS) {
                uret = fucn_uret;
            }
            initflag_clear(USBH_FLAG_FLOW_CLASS_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host class has been deinited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        if (initflag_check(USBH_FLAG_FLOW_HCD_INIT) != 0U) {
            fucn_uret = usbh_hcd_deinit(&usbh_class_config);
            if (fucn_uret != USB_ERR_SUCCESS) {
                uret = fucn_uret;
            }
            initflag_clear(USBH_FLAG_FLOW_HCD_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "hcd has been deinited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        if (initflag_check(USBH_FLAG_FLOW_USBX_INIT) != 0U) {
            fucn_uret = usbh_deinit_usbx();
            if (fucn_uret != USB_ERR_SUCCESS) {
                uret = fucn_uret;
            }
            initflag_clear(USBH_FLAG_FLOW_USBX_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "usbx has been deinited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        if (initflag_check(USBH_FLAG_FLOW_HW_INIT) != 0U) {
            hardware_deinit();
            initflag_clear(USBH_FLAG_FLOW_HW_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host hw has been deinited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        initflag_clear(USBH_FLAG_FLOW_HOST_INIT);
    } else {
        // print debug message
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "AmbaUSBH_System_ClassStop is called before, flag = 0x%x", flag_init, 0, 0, 0, 0);
    }

    return uret;
}

static UINT32 test_mode_dev_iproduct_set(UX_DEVICE *UsbxDevice, UINT32 TestMode)
{
    UINT32 uret = USB_ERR_SUCCESS;

    switch (TestMode) {
    case USBH_TEST_MODE_SE0_NAK:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_SE0_NAK;
        break;
    case USBH_TEST_MODE_J_STATE:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_J;
        break;
    case USBH_TEST_MODE_K_STATE:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_K;
        break;
    case USBH_TEST_MODE_PACKET:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_PACKET;
        break;
    case USBH_TEST_MODE_FORCE_ENABLE:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_FORCE_ENABLE;
        break;
    case USBH_TEST_MODE_SUSPEND:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_SR;
        break;
    case USBH_TEST_MODE_RESUME:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_SR_S2;
        break;
    case USBH_TEST_MODE_GET_DESCRIPTOR:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_GET_DEV_DESCRIPTOR;
        break;
    case USBH_TEST_MODE_SET_FEATURE_S1:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_SET_FEATURE;
        break;
    case USBH_TEST_MODE_SET_FEATURE_S2:
        UsbxDevice->ux_device_descriptor.idProduct = UX_HCD_EHSET_TEST_SET_FEATURE_STEP_2;
        break;
    default:
        uret = USB_ERR_PARAMETER_INVALID;
        break;
    }
    return uret;
}

/**
 * The application calls this API to enable the USB test mode for USB compliance testing. Ambarella suggests to reboot system for each test item.
 * @param TestMode [Input] test mode:
- 1: USBH_TEST_MODE_J_STATE
- 2: USBH_TEST_MODE_K_STATE
- 3: USBH_TEST_MODE_SE0_NAK
- 4: USBH_TEST_MODE_PACKET
- 5: USBH_TEST_MODE_FORCE_ENABLE
- 6: USBH_TEST_MODE_SUSPEND
- 7: USBH_TEST_MODE_RESUME
- 8: USBH_TEST_MODE_GET_DESCRIPTOR
- 9: USBH_TEST_MODE_SET_FEATURE_S1
- 10: USBH_TEST_MODE_ET_FEATURE_S2
*/
UINT32 AmbaUSBH_SystemSetTestMode(UINT32 TestMode)
{
    UX_DEVICE *usbx_device;
    UX_DEVICE fake_ux_device;
    UINT32 uret = USB_ERR_SUCCESS;
    UINT32 flag_leave  = 0;

    if (USBH_SystemIsInit() != 0U) {

        // Try to obtain the 1st connected device.
        // If there's no connect device, create a fake device instance.
        if (_ux_host_stack_device_get(0, &usbx_device) != (UINT32)UX_SUCCESS) {
            if ((TestMode == USBH_TEST_MODE_FORCE_ENABLE) ||
                (TestMode == USBH_TEST_MODE_SE0_NAK) ||
                (TestMode == USBH_TEST_MODE_J_STATE) ||
                (TestMode == USBH_TEST_MODE_K_STATE) ||
                (TestMode == USBH_TEST_MODE_PACKET)) {
                UINT32 hcd_index;
                UX_HCD *usbx_hcd = NULL;
                USB_UtilityMemorySet(&fake_ux_device, 0, sizeof(UX_DEVICE));
                usbx_device                          = &fake_ux_device;
                usbx_device->ux_device_port_location = 0;

                for (hcd_index = 0; hcd_index < _ux_system_host->ux_system_host_registered_hcd; hcd_index++) {
                    // Pickup HCD pointer.
                    usbx_hcd = &_ux_system_host->ux_system_host_hcd_array[hcd_index];
                    if (_ux_utility_string_compare((const char*)usbx_hcd->ux_hcd_name, (const char*)host_hcd_ehci_name,
                                                   (UINT32)_ux_utility_string_length_get(host_hcd_ehci_name)) == 0) {
                        break;
                    }
                }

                if (hcd_index == _ux_system_host->ux_system_host_registered_hcd) {
                    flag_leave  = 1;
                    uret = USB_ERR_EHCI_NO_HCD_REGISTERED;
                } else {
                    usbx_device->ux_device_hcd = usbx_hcd;
                }
            } else {
                flag_leave  = 1;
                uret = USB_ERR_PARAMETER_INVALID;
            }
        }

        if (flag_leave == 0U) {
            uret = test_mode_dev_iproduct_set(usbx_device, TestMode);
            if (uret == USB_ERR_SUCCESS) {
                uret = _ux_host_stack_ehset(usbx_device, 0);
                if (uret != (UINT32)UX_SUCCESS) {
                    // shall not be here.
                }
            }
        }
    } else {
        uret = USB_ERR_NO_INSTANCE;
    }
    return uret;
}
/** @} */
