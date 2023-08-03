/**
 *  @file AmbaUSBD_System.c
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
 *  @details USB system functions.
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
#include <AmbaUSBD_Descriptor.h>

typedef struct {
    USBD_SYS_CONFIG_s   SysConfig;
    USBD_CLASS_CONFIG_s ClassConfig;
    UINT8              *CtrlrMemPtr;
    UINT32              CtrlrMemSize;
} USBD_INSTANCE_s;

#define USBD_FLAG_FLOW_SYSTEM_CFG_INIT ((UINT32)0x80000000UL)
#define USBD_FLAG_FLOW_CLASS_CFG_INIT  ((UINT32)0x40000000UL)
#define USBD_FLAG_FLOW_CONNECT_INIT    ((UINT32)0x00000001UL)
#define USBD_FLAG_FLOW_HW_INIT         ((UINT32)0x00000002UL)
#define USBD_FLAG_FLOW_ISR_INIT        ((UINT32)0x00000004UL)
#define USBD_FLAG_FLOW_USBX_INIT       ((UINT32)0x00000008UL)
#define USBD_FLAG_FLOW_CLASS_INIT      ((UINT32)0x00000010UL)
#define USBD_FLAG_FLOW_DATA_CONN_EN    ((UINT32)0x00000040UL)

static USBD_INSTANCE_s usbd_instance __attribute__((section(".bss.noinit")));
static UINT32 flag_init = 0;

static void usbd_initflag_set(UINT32 Flag)
{
    flag_init |= Flag;
}

static UINT32 usbd_initflag_check(UINT32 Flag)
{
    UINT32 uret;

    if ((flag_init & Flag) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}

static void usbd_initflag_clear(UINT32 Flag)
{
    flag_init &= (~Flag);
}

static void usbd_store_system_config(const USBD_SYS_CONFIG_s *Config, UINT32 ControllerMemSize)
{
    usbd_instance.SysConfig = *Config;

    usbd_instance.CtrlrMemPtr  = NULL;
    usbd_instance.CtrlrMemSize = 0;

    if (ControllerMemSize > 0U) {
        if (usbd_instance.SysConfig.StackNonCacheMemorySize > ControllerMemSize) {
            usbd_instance.CtrlrMemPtr  = usbd_instance.SysConfig.StackNonCacheMemory;
            usbd_instance.CtrlrMemSize = ControllerMemSize;

            // twick non-cache memory size
            usbd_instance.SysConfig.StackNonCacheMemorySize -= ControllerMemSize;
            usbd_instance.SysConfig.StackNonCacheMemory = &usbd_instance.SysConfig.StackNonCacheMemory[ControllerMemSize];
        }
    }
}

static void usbd_store_class_config(const USBD_CLASS_CONFIG_s *Config)
{
    usbd_instance.ClassConfig = *Config;
}

static UINT32 usbd_check_host_mode(void)
{
    return USBH_SystemIsInit();
}

static UINT32 usbd_init_usbx(void)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (usbd_check_host_mode() == 0U) {
        uret = ux_system_initialize(usbd_instance.SysConfig.StackCacheMemory,
                                    usbd_instance.SysConfig.StackCacheMemorySize,
                                    usbd_instance.SysConfig.StackNonCacheMemory,
                                    usbd_instance.SysConfig.StackNonCacheMemorySize);

        if (uret != (UINT32)UX_SUCCESS) {
            if (uret == (UINT32)UX_MUTEX_ERROR) {
                uret = USBX_ERR_SYSTEM_INIT_FAIL;
            } else {
                uret = USBX_ERR_FFFF;
            }
        }
    }

    return uret;
}

static UINT32 usbd_deinit_usbx(void)
{
    return _ux_system_destroy();
}

static UINT32 usbd_init_hw(void)
{
    return USBD_UdcInit();
}

static UINT32 usbd_deinit_hw(void)
{
    return USBD_UdcDeInit();
}

static UINT32 usbd_init_isr(void)
{
    return USBD_IntInit();
}

static UINT32 usbd_deinit_isr(void)
{
    return USBD_IntDeInit();
}

static UINT32 usbd_init_class(void)
{
    UINT32 uret = USB_ERR_SUCCESS;
    USBD_DESC_CUSTOM_INFO_s desc_info;

    if (usbd_instance.ClassConfig.ClassID == USBD_CLASS_MSC) {
        // setup timeout for transfers
        uxd_ep_xfer_timeout_set(3000);
    } else if (usbd_instance.ClassConfig.ClassID == USBD_CLASS_MTP) {
        // setup timeout for transfers
        uxd_ep_xfer_timeout_set(3000);
    } else if (usbd_instance.ClassConfig.ClassID == USBD_CLASS_CDC_ACM) {
        // setup timeout for transfers
        uxd_ep_xfer_timeout_set(3000);
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    else if (usbd_instance.ClassConfig.ClassID == USBD_CLASS_UVC) {
        // no more action here.
    }
#endif
    else if (usbd_instance.ClassConfig.ClassID == USBD_CLASS_USER) {
        // no more action here.
    } else {
        uret = USB_ERR_INVALID_CLASS_ID;
    }

    if (uret == USB_ERR_SUCCESS) {
        // initialize descriptor
        uret = USBD_DescriptorInit(usbd_instance.ClassConfig.Framework);
        if (uret == USB_ERR_SUCCESS) {
            // patch descriptor if needed
            USBD_DescriptorInfoGet(&desc_info);

            // call start function
            if (usbd_instance.ClassConfig.ClassStartCb != NULL) {
                uret = usbd_instance.ClassConfig.ClassStartCb(&desc_info);
            } else {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "usbd_init_class(): no class start function.");
                uret = USB_ERR_USBD_NO_CLASS_START_FUNCTION;
            }
        }
    }

    return uret;
}

static UINT32 usbd_deinit_class(void)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (usbd_instance.ClassConfig.ClassStopCb != NULL) {
        uret = usbd_instance.ClassConfig.ClassStopCb();
    } else {
        // impossible here. it should be protected by AmbaUSBD_SystemClassStart()
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "usbd_deinit_class(): no class stop function.");
        uret = USB_ERR_USBD_NO_CLASS_STOP_FUNCTION;
    }

    USBD_DescriptorInfoClear();
    return uret;
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to convert USBX error code to Ambarella error code.
 * */
UINT32 USB_UtilityUXCode2AmbaCode(UINT32 Code)
{
    UINT32 uret = USBX_ERR_FAIL;

    if (Code == (UINT32)UX_MEMORY_INSUFFICIENT) {
        uret = USBX_ERR_NO_MEMORY;
    } else if (Code == (UINT32)UX_DESCRIPTOR_CORRUPTED) {
        uret = USBX_ERR_DESCRIPTOR_CORRUPTED;
    } else if (Code == (UINT32)UX_MUTEX_ERROR) {
        uret = USBX_ERR_MUTEX_ERROR;
    } else if (Code == (UINT32)UX_SEMAPHORE_ERROR) {
        uret = USBX_ERR_SEMAPHORE_ERROR;
    } else if (Code == (UINT32)UX_THREAD_ERROR) {
        uret = USBX_ERR_THREAD_ERROR;
    } else if (Code == (UINT32)UX_EVENT_ERROR) {
        uret = USBX_ERR_EVENT_ERROR;
    } else if (Code == (UINT32)UX_HOST_CLASS_ALREADY_INSTALLED) {
        uret = USBX_ERR_CLASS_ALREADY_INSTALLED;
    } else if (Code == (UINT32)UX_MEMORY_ARRAY_FULL) {
        uret = USBX_ERR_NO_AVAILABLE_CLASS_ARRAY;
    } else {
        uret = USBX_ERR_FAIL;
    }
    return uret;
}

void USBD_SystemAppEventNotify(UINT32 EventCode, UINT32 Param1, UINT32 Param2, UINT32 Param3)
{
    USBD_SYS_CONFIG_s *sys_config;
    UINT32 uret = USBD_SystemConfigGet(&sys_config);

    if (uret == USB_ERR_SUCCESS) {
        if (sys_config != NULL) {
            if (sys_config->SystemEventCallback != NULL) {
                if (sys_config->SystemEventCallback(EventCode, Param1, Param2, Param3) != 0U) {
                    // ignore error
                }
            }
        }
    }
    return;
}

/**
 * Called by driver to get system configuration.
 * */
UINT32 USBD_SystemCtrlrMemGet(UINT8 **Ptr, UINT32 *Size)
{
    UINT32 uret;

    if ((flag_init & USBD_FLAG_FLOW_SYSTEM_CFG_INIT) == 0U) {
        uret    = USB_ERR_NO_INIT;
        *Ptr = NULL;
        *Size = 0;
    } else {
        uret    = USB_ERR_SUCCESS;
        *Ptr    = usbd_instance.CtrlrMemPtr;
        *Size   = usbd_instance.CtrlrMemSize;
    }

    return uret;
}

/**
 * Called by driver to get system configuration.
 * */
UINT32 USBD_SystemConfigGet(USBD_SYS_CONFIG_s **Config)
{
    UINT32 uret;

    if ((flag_init & USBD_FLAG_FLOW_SYSTEM_CFG_INIT) == 0U) {
        uret    = USB_ERR_NO_INIT;
        *Config = NULL;
    } else {
        uret    = USB_ERR_SUCCESS;
        *Config = &usbd_instance.SysConfig;
    }

    return uret;
}
/**
 * Called by driver to get current class configuration.
 * */
UINT32 USBD_SystemClassConfigGet(USBD_CLASS_CONFIG_s **Config)
{
    UINT32 uret;

    if ((flag_init & USBD_FLAG_FLOW_CLASS_CFG_INIT) == 0U) {
        uret    = USB_ERR_NO_INIT;
        *Config = NULL;
    } else {
        uret    = USB_ERR_SUCCESS;
        *Config = &usbd_instance.ClassConfig;
    }

    return uret;
}
/**
 * Called by driver to check if USB device class is configured.
 * */
UINT32 USBD_SystemIsClassConfiged(void)
{
    UINT32 uret;

    if ((flag_init & USBD_FLAG_FLOW_CLASS_CFG_INIT) == 0U) {
        uret    = USB_ERR_NO_INIT;
    } else {
        uret    = USB_ERR_SUCCESS;
    }
    return uret;
}

/**
 * Called by driver to check if USB device system is initialized.
 * */
UINT32 USBD_SystemIsInit(void)
{
    UINT32 uret;

    if (usbd_initflag_check(USBD_FLAG_FLOW_SYSTEM_CFG_INIT) != 0U) {
        uret = 1;
    } else {
        uret = 0;
    }

    return uret;
}

static UINT32 usbd_check_parameters(const USBD_SYS_CONFIG_s *pConfig)
{
    UINT32 uret = 0;
    if ((pConfig == NULL) ||
        (pConfig->StackCacheMemory == NULL) ||
        (pConfig->StackNonCacheMemory == NULL) ||
        (pConfig->SystemEventCallback == NULL) ||
        (pConfig->SystemTaskCreateCb == NULL) ||
        (pConfig->SystemTaskSuspendCb == NULL)) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if (pConfig->CoreMask == 0U) {
        uret = USB_ERR_COREMASK_ZERO;
    } else if ((pConfig->StackCacheMemorySize == 0U) ||
               (pConfig->StackNonCacheMemorySize == 0U) ||
               (pConfig->StackCacheMemorySize < (15UL*1024UL))) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else if ((pConfig->InitSpeed >= 2U) || (pConfig->SetCfgThd == 0U)) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else {
#if USBCTRLR_MEMORY_SIZE > 0U
        UINT32 size = USBCTRLR_MEMORY_SIZE;
        if (pConfig->StackNonCacheMemorySize < size) {
            uret = USB_ERR_PARAMETER_INVALID;
        }
#endif
        // do nothing
    }
    return uret;
}

/** @} */

/** \addtogroup Public_APIs
 *  @{
 * */
/**
 * This function is used to initialize the USB system, register the system events Callbacks and start the Vbus detection task.
 * @param pConfig [Input] System configuration
 * */
UINT32 AmbaUSBD_SystemInit(const USBD_SYS_CONFIG_s *pConfig)
{
    UINT32 uret;

    if (USBH_SystemIsInit() == 0U) {
        if (usbd_initflag_check(USBD_FLAG_FLOW_SYSTEM_CFG_INIT) == 0U) {
            uret = usbd_check_parameters(pConfig);
            if (uret == 0U) {
                // initialize RTSL
                USB_UtilityRTSLInit();

                // Store the configuration.
                usbd_store_system_config(pConfig, USBCTRLR_MEMORY_SIZE);
                usbd_initflag_set(USBD_FLAG_FLOW_SYSTEM_CFG_INIT);

                // Initialize the Utility.
                USB_UtilityCoreMaskSet(usbd_instance.SysConfig.CoreMask);

                // Initialize Connect Change Detection
                if (usbd_initflag_check(USBD_FLAG_FLOW_CONNECT_INIT) == 0U) {
                    uret = USBD_ConnectChgInit();
                    if (uret == USB_ERR_SUCCESS) {
                        usbd_initflag_set(USBD_FLAG_FLOW_CONNECT_INIT);
                    } else {
                        // error handling
                        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Connect Change Init Fail, uret = 0x%x", uret, 0, 0, 0, 0);
                    }
                } else {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "Vbus has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
                    uret = USB_ERR_SUCCESS;
                }
            }
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "Init has been done before, flag = 0x%x", flag_init, 0, 0, 0, 0);
            uret = USB_ERR_SUCCESS;
        }

        // disconnect the data line.
        USBD_UdcDeviceDataConnSet(0);
        usbd_initflag_clear(USBD_FLAG_FLOW_DATA_CONN_EN);
    } else {
        uret = USB_ERR_HOST_IS_RUNNING;
    }
    return uret;
}
/** @} */

static UINT32 class_start_check_param(const USBD_CLASS_CONFIG_s* pClassConfig)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (usbd_initflag_check(USBD_FLAG_FLOW_SYSTEM_CFG_INIT) == 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "system init is not called before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        uret = USB_ERR_NO_INIT;
    } else if (usbd_initflag_check(USBD_FLAG_FLOW_CLASS_CFG_INIT) == 1U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "class start has been done before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        uret = USB_ERR_SUCCESS;
    } else if ((pClassConfig == NULL) || (pClassConfig->Framework == NULL)) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((pClassConfig->Framework->DescFrameworkFs == NULL) ||
               (pClassConfig->Framework->DescFrameworkHs == NULL) ||
               (pClassConfig->Framework->StrFramework == NULL) ||
               (pClassConfig->Framework->LangIDFramework == NULL)) {
        uret = USB_ERR_PARAMETER_NULL;
    } else if ((pClassConfig->Framework->DescSizeFs == 0U) ||
               (pClassConfig->Framework->DescSizeHs == 0U) ||
               (pClassConfig->Framework->StrSize == 0U) ||
               (pClassConfig->Framework->LangIDSize == 0U)) {
        uret = USB_ERR_PARAMETER_INVALID;

    } else if ((pClassConfig->ClassID == USBD_CLASS_NONE) || (pClassConfig->ClassID >= USBD_CLASS_NUM)) {
        uret = USB_ERR_INVALID_CLASS_ID;
    } else if (pClassConfig->ClassStartCb == NULL) {
        uret = USB_ERR_USBD_NO_CLASS_START_FUNCTION;
    } else if (pClassConfig->ClassStopCb == NULL) {
        uret = USB_ERR_USBD_NO_CLASS_STOP_FUNCTION;
    } else {
        // pass vcast check
    }

    return uret;
}

/** \addtogroup Public_APIs
 *  @{
 * */
/**
 * This function starts the Ambarella USB device with the specified USB class.
 * @param pClassConfig [Input] configuration information.
*/
UINT32 AmbaUSBD_SystemClassStart(const USBD_CLASS_CONFIG_s* pClassConfig)
{
    UINT32 uret = class_start_check_param(pClassConfig);

    if (uret == USB_ERR_SUCCESS) {

        UINT32 EnableDataLine = 1;

        // Store the configuration.
        usbd_store_class_config(pClassConfig);
        usbd_initflag_set(USBD_FLAG_FLOW_CLASS_CFG_INIT);

        // Initialize the USBX
        if (usbd_initflag_check(USBD_FLAG_FLOW_USBX_INIT) == 0U) {
            uret = usbd_init_usbx();
            if (uret == USB_ERR_SUCCESS) {
                usbd_initflag_set(USBD_FLAG_FLOW_USBX_INIT);
            } else {
                EnableDataLine = 0;
            }
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "usbx has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        if (uret == USB_ERR_SUCCESS) {
            // Initialize the USB HW
            if (usbd_initflag_check(USBD_FLAG_FLOW_HW_INIT) == 0U) {
                uret = usbd_init_hw();
                if (uret == USB_ERR_SUCCESS) {
                    usbd_initflag_set(USBD_FLAG_FLOW_HW_INIT);
                } else {
                    EnableDataLine = 0;
                }
            } else {
                // print debug message
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB HW has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
            }
        }

        if (uret == USB_ERR_SUCCESS) {
            // Initialize the USB Class
            if (usbd_initflag_check(USBD_FLAG_FLOW_CLASS_INIT) == 0U) {
                uret = usbd_init_class();
                if (uret == USB_ERR_SUCCESS) {
                    usbd_initflag_set(USBD_FLAG_FLOW_CLASS_INIT);
                } else {
                    EnableDataLine = 0;
                }
            } else {
                // print debug message
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB class has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
            }
        }

        if (uret == USB_ERR_SUCCESS) {
            // Initialize the USB ISR
            if (usbd_initflag_check(USBD_FLAG_FLOW_ISR_INIT) == 0U) {
                uret = usbd_init_isr();
                if (uret == USB_ERR_SUCCESS) {
                    usbd_initflag_set(USBD_FLAG_FLOW_ISR_INIT);
                } else {
                    EnableDataLine = 0;
                }
            } else {
                // print debug message
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB ISR has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
            }
        }

        // Connect the data line.
        if (EnableDataLine == 1U) {
            USBD_UdcDeviceDataConnSet(1);
            usbd_initflag_set(USBD_FLAG_FLOW_DATA_CONN_EN);
        }
    }

    return uret;
}
/**
 * This function releases all Ambarella USB device resources except for Vbus detection and disables USB PHY power.
 * This API can be called only if AmbaUSBD_SystemClassStart() has been issued.
*/
UINT32 AmbaUSBD_SystemClassStop(void)
{
    UINT32 func_ret = USB_ERR_SUCCESS;
    UINT32 uret = USB_ERR_SUCCESS;

    if (usbd_initflag_check(USBD_FLAG_FLOW_SYSTEM_CFG_INIT) == 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "system init is not called before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        uret = USB_ERR_NO_INIT;
    } else if (usbd_initflag_check(USBD_FLAG_FLOW_CLASS_CFG_INIT) == 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "class start is not called before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        uret = USB_ERR_SUCCESS;
    } else {
        // de-initialize the USB ISR
        if (usbd_initflag_check(USBD_FLAG_FLOW_ISR_INIT) == 1U) {
            func_ret = usbd_deinit_isr();
            if (func_ret != USB_ERR_SUCCESS) {
                uret = func_ret;
            }
            usbd_initflag_clear(USBD_FLAG_FLOW_ISR_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB ISR has been de-inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        // disconnect the data line.
        USBD_UdcDeviceDataConnSet(0);
        usbd_initflag_clear(USBD_FLAG_FLOW_DATA_CONN_EN);

        // de-initialize the USB Class
        if (usbd_initflag_check(USBD_FLAG_FLOW_CLASS_INIT) == 1U) {
            // Destory the interface. Destory the endpoints. Suspend the USBX threads.
            UINT32 IsConfigure = USBD_UdcIsConfigured();
            func_ret = _ux_device_stack_disconnect();
            if ((func_ret != USB_ERR_SUCCESS) && (IsConfigure != 0U)) {
                // _ux_device_stack_disconnect() also return error when system is not configured.
                // in this case it is success.
                uret = USBX_ERR_CLASS_DEACTIVATE_FAIL;
            }

            // Delete the USBX threads.
            func_ret = usbd_deinit_class();
            if (func_ret != USB_ERR_SUCCESS) {
                uret = func_ret;
            }
            usbd_initflag_clear(USBD_FLAG_FLOW_CLASS_INIT);

        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB class has been de-inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        // de-initialize the USB HW
        if (usbd_initflag_check(USBD_FLAG_FLOW_HW_INIT) == 1U) {
            func_ret = usbd_deinit_hw();
            if (func_ret != USB_ERR_SUCCESS) {
                uret = func_ret;
            }
            usbd_initflag_clear(USBD_FLAG_FLOW_HW_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB HW has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        // de-initialize the USBX
        if (usbd_initflag_check(USBD_FLAG_FLOW_USBX_INIT) == 1U) {
            func_ret = usbd_deinit_usbx();
            if (func_ret != USB_ERR_SUCCESS) {
                uret = USBX_ERR_SYSTEM_DEINIT_FAIL;
            }
            usbd_initflag_clear(USBD_FLAG_FLOW_USBX_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "usbx has been inited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
        }

        usbd_initflag_clear(USBD_FLAG_FLOW_CLASS_CFG_INIT);

    }

    return uret;
}

/**
 * This function releases all Ambarella USB device resource including Vbus detection.
 * This API can be called only if AmbaUSBD_SystemInit() has been issued.
*/
UINT32 AmbaUSBD_SystemDeInit(void)
{
    UINT32 uret;

    uret = AmbaUSBD_SystemClassStop();
    if (uret != USB_ERR_SUCCESS) {
        // error handling
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "class deinit fail, uret = 0x%x, flag = 0x%x", uret, flag_init, 0, 0, 0);
    } else {
        if (usbd_initflag_check(USBD_FLAG_FLOW_SYSTEM_CFG_INIT) != 0U) {
            // De-initialize Connect Change Detection
            if (usbd_initflag_check(USBD_FLAG_FLOW_CONNECT_INIT) != 0U) {
                uret = USBD_ConnectChgDeInit();
                if (uret == USB_ERR_SUCCESS) {
                    usbd_initflag_clear(USBD_FLAG_FLOW_CONNECT_INIT);
                } else {
                    // error handling
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Connect Change DeInit Fail, uret = 0x%x", uret, 0, 0, 0, 0);
                }
            } else {
                // print debug message
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "Vbus has been deinited before, flag = 0x%x", flag_init, 0, 0, 0, 0);
                uret = USB_ERR_SUCCESS;
            }
            usbd_initflag_clear(USBD_FLAG_FLOW_SYSTEM_CFG_INIT);
        } else {
            // print debug message
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "system deinit has been done before, flag = 0x%x", flag_init, 0, 0, 0, 0);
            uret = USB_ERR_SUCCESS;
        }
    }

    return uret;
}
/**
 * It is used to detect the charger after the VBUS is detected.
 * Please note that it can only detect chargers which support USB Battery Charger 1.1.
 * For chargers which do not support USB Battery Charger 1.1, the result of this API is unknown.
 * The charger detection API should be called after Vbus detection event is issued.
 * @param T1 [Input] The time (ms) for detection taking effect. 0 means “detect right now”.
 * @param T2 [Input] The detection period (ms). Shall be larger than 10ms.
 * @param pChargerType [Output] Charge detection result:
- 0: SDP (Standard Downstream Port.) Draw 500mA at maixmum.
- 1: DCP (Dedicate Charger Port.) Draw 1.5A at maximum based on battery charger 1.1 spec.
- 2: CDP (Charger Downstream Port.) Draw 1.5A at maximum based on battery charger 1.1 spec.
- 3: No data contact detected.
*/
UINT32 AmbaUSBD_SystemChargerDetection(UINT32 T1, UINT32 T2, UINT32 *pChargerType)
{
#if 0
    UINT32 contact = 0;
#endif
    UINT32 uret;

    if (pChargerType == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else {
        // all CV chips don't support charger detection
        (void)(T1);
        (void)(T2);
        *pChargerType = (UINT32)USBD_CHGDET_NO_CONTACT;
        uret = USB_ERR_NO_INSTANCE;
#if 0
        uret = USB_PhyDataContactDetection(T1, T2, &contact);

        if (uret == USB_ERR_SUCCESS) {
            if (contact == 1U) {
                USB_PhyChargerDetection(pChargerType);
            } else {
                *pChargerType = (UINT32)USBD_CHGDET_NO_CONTACT;
            }
        } else {
            *pChargerType = (UINT32)USBD_CHGDET_NO_CONTACT;
        }
#endif
    }

    return uret;
}
/**
 * This function is used to get the current USB device connection speed.
 * @param pSpeed [Output] current USB device connection speed:
- 0: Full speed
- 1: High speed
*/
UINT32 AmbaUSBD_SystemGetConnectSpeed(UINT32 *pSpeed)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (pSpeed == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else {
        *pSpeed = USBD_UdcEnumSpeedGet();
    }
    return uret;
}
/**
 * This function is used to check if the USB cable is inserted or not by detecting the VBUS connection.
 * This API must be called after AmbaUSBD_SystemInit().
 * @param pVbusStatus [Output] current USB device VBUS status:
- 0: USB cable is removed.
- 1: USB cable is inserted.
*/
UINT32 AmbaUSBD_SystemGetVbusStatus(UINT32 *pVbusStatus)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (pVbusStatus == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else {
        *pVbusStatus = USBD_ConnectChgVbusStatusGet();
    }
    return uret;
}
/**
 * This function is used to report if the device is recognized and configured by the host.
 * @param pIsConfigured    [Output] current USB device status:<br>
- 0: Device is not configured by Host.
- 1: Device is configured by Host.
*/
UINT32 AmbaUSBD_SystemIsConfigured(UINT32 *pIsConfigured)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (pIsConfigured == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else {
        *pIsConfigured = USBD_UdcIsConfigured();
    }
    return uret;
}

/**
 * This function is used to get the version control system string of USB library.
 * @param pVersionInfo [Output] Version information
*/
UINT32 AmbaUSB_SystemGetVersionInfo(USB_VER_INFO_s *pVersionInfo)
{
    UINT32 uret = USB_ERR_SUCCESS;

    if (pVersionInfo == NULL) {
        uret = USB_ERR_PARAMETER_INVALID;
    } else {
        pVersionInfo->MajorVersion = 2;
        pVersionInfo->MinorVersion = 0;
    }
    return uret;
}
/** @} */
