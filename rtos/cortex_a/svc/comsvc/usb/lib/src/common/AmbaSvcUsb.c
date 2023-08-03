/**
 *  @file AmbaSvcUsb.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details USB Device main application.
 */

#include "AmbaPrint.h"
#include "AmbaSvcUsb.h"
#include "AmbaUSB_ErrCode.h"
#include <AmbaGPIO_Def.h>
#include <AmbaGPIO.h>
#include "AmbaSvcWrap.h"

#include "MSCDevice.h"
#include "MTPDevice.h"

#include "MTPDesc.c"
#include "SvcUsb.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "UVCDesc.c"
#endif

#define AMBAUSB_TEST_VBUS_TASK_STACK_SIZE       (16 * 1024)
#define AMBAUSB_TEST_ISR_TASK_STACK_SIZE        (8 * 1024)
#define AMBAUSB_TEST_CTRL_TASK_STACK_SIZE       (8 * 1024)

typedef struct {
    UINT8   *pCaMem;
    UINT32  CaSize;
    UINT8   *pNcMem;
    UINT32  NcSize;
} SVC_USB_CFG_s;

static SVC_USB_CFG_s g_UsbCfg = {.pCaMem = NULL, .CaSize = 0U, .pNcMem = NULL, .NcSize = 0U};

static AMBA_KAL_TASK_t  VBusDetTask;
static UINT32           AppUsbd_CurrentClassID = AMBA_USBD_CLASS_MSC;
static UINT32           AppDeviceInited = 0U;

static UINT32 TaskCreate(AMBA_KAL_TASK_t *pTask,
                         char *           pTaskName,
                         UINT32           Priority,
                         USB_TASK_ENTRY_f EntryFunction,
                         void*            EntryArg,
                         void *           pStackBase,
                         UINT32           StackByteSize,
                         UINT32           AutoStart,
                         UINT32           AffinityMask)
{
    // create taks with no start.
    UINT32 sRet;
    UINT32 Ret;
    static UINT32 CoreMask = AMBA_KAL_CPU_CORE_MASK;

    AmbaMisra_TouchUnused(EntryArg);

    sRet = AmbaKAL_TaskCreate(pTask,
                              pTaskName,
                              Priority,
                              EntryFunction,
                              EntryArg,
                              pStackBase,
                              StackByteSize,
                              0U);

    if (sRet == TX_SUCCESS) {
#ifndef AMBA_KAL_NO_SMP
        UINT32 TmpAffMsk = AffinityMask;
        TmpAffMsk &= CoreMask;
        if (AmbaKAL_TaskSetSmpAffinity(pTask, TmpAffMsk) != TX_SUCCESS) {
            Ret = USB_ERR_FAIL;
        } else {
            if (AutoStart == 1U) {
                // kick task start.
                if (AmbaKAL_TaskResume(pTask) != TX_SUCCESS) {
                    Ret = USB_ERR_FAIL;
                } else {
                    Ret = USB_ERR_SUCCESS;
                }
            } else {
                Ret = USB_ERR_SUCCESS;
            }
        }
#else
        (void)AffinityMask;
        (void)CoreMask;

        if (AutoStart == 1U) {
            // kick task start.
            if (AmbaKAL_TaskResume(pTask) != TX_SUCCESS) {
                Ret = USB_ERR_FAIL;
            } else {
                Ret = USB_ERR_SUCCESS;
            }
        } else {
            Ret = USB_ERR_SUCCESS;
        }
#endif
    } else {
        Ret = USB_ERR_FAIL;
    }

    return Ret;
}

static UINT32 TaskResume(AMBA_KAL_TASK_t *pTask)
{
    UINT32 uRet = 0;

    if (AmbaKAL_TaskResume(pTask) != TX_SUCCESS) {
        uRet = USB_ERR_FAIL;
    }
    return uRet;
}

static UINT32 TaskSuspend(AMBA_KAL_TASK_t *pTask)
{
    UINT32 uRet = AmbaKAL_TaskSuspend(pTask);

    if (uRet != TX_SUCCESS) {
        AmbaPrint_PrintUInt5("TaskSuspend(): return 0x%X", uRet, 0, 0, 0, 0);
        uRet = USB_ERR_FAIL;
    }
    return uRet;
}

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
static UINT32 UsbConfigurValue = 0;
static int UsbConfigured = 0;
UINT32 AmbaSvcUsb_SetConfigured(int value)
{
    UsbConfigured = value;

    return 0;
}

int AmbaSvcUsb_GetConfigured(UINT32 *value)
{
    if (UsbConfigured && value != NULL) {
        *value = UsbConfigurValue;
    }

    return UsbConfigured;
}
#endif

static UINT32 USB_SystemEventCallback(UINT32 Code, UINT32 Param1, UINT32 Param2, UINT32 Param3)
{
    static UINT32 ConnectCnt = 0;

    AmbaMisra_TouchUnused(&Code);
    AmbaMisra_TouchUnused(&Param1);
    AmbaMisra_TouchUnused(&Param2);
    AmbaMisra_TouchUnused(&Param3);

    switch (Code) {
    case USBD_SYSTEM_EVENT_CONNECT:
        AmbaPrint_PrintUInt5("Vbus Connect!! %d time(s)", (ConnectCnt + 1U), 0, 0, 0, 0);
        ConnectCnt++;
        break;
    case USBD_SYSTEM_EVENT_DISCONNECT:
        AmbaPrint_PrintUInt5("Vbus Disconnect!! %d time(s)", ConnectCnt, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_CONFIGURED:
        AmbaPrint_PrintUInt5("USBD System Configured, Index = %d", Param1, 0, 0, 0, 0);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        UsbConfigurValue = Param1;
        UsbConfigured = 1;
#endif
        break;
    case USBD_SYSTEM_EVENT_SUSPENDED:
        AmbaPrint_PrintUInt5("USBD Suspended", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_RESUMED:
        AmbaPrint_PrintUInt5("USBD Resumed", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_RESET:
        AmbaPrint_PrintUInt5("USBD Reset", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_SET_FEATURED:
        AmbaPrint_PrintUInt5("USBD Set Feature", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_CLEAR_FEATURED:
        AmbaPrint_PrintUInt5("USBD Clr Feature", 0, 0, 0, 0, 0);
        break;
    case USBD_SYSTEM_EVENT_SET_INTERFACE:
        AmbaPrint_PrintUInt5("USBD Set Interface, Interface = %d, Alternate Setting = %d", Param1, Param2, 0, 0, 0);
        break;
    default:        
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintUInt5("USBD system error notification, ErrCode = %d, UsrSpecific = %d", Param1, Param2, 0, 0, 0);
#endif
        break;
    }
    return 0;
}

static UINT32 USB_SystemTaskCreateCb(UINT32 Code, USB_TASK_ENTRY_f TaskEntry, void* Arg, AMBA_KAL_TASK_t **pTask)
{
    UINT32 uRet                       = 0;
    static char UsbdVbusDetTaskName[] = "AmbaUSBD_RefVbusDetection";
    static char UsbdIsrTaskName[]     = "AmbaUSBD_RefIsr";
    static char UsbdCtrlTaskName[]    = "AmbaUSBD_RefCtrl";
    static UINT8 VbusTaskStackMem[AMBAUSB_TEST_VBUS_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static UINT8 CtrlTaskStackMem[AMBAUSB_TEST_CTRL_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static UINT8 IsrTaskStackMem[AMBAUSB_TEST_ISR_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static AMBA_KAL_TASK_t UsbdCtrlTask;
    static AMBA_KAL_TASK_t UsbdIsrTask;

    AmbaMisra_TouchUnused(&Code);

    switch (Code) {
    case 0:
        if (*pTask == NULL) {
            // new task creation
            uRet = TaskCreate(&VBusDetTask,
                              UsbdVbusDetTaskName,
                              254,
                              TaskEntry,
                              Arg,
                              VbusTaskStackMem,
                              AMBAUSB_TEST_VBUS_TASK_STACK_SIZE,
                              1U, //AMBA_KAL_AUTO_START
                              0x01);
            *pTask = &VBusDetTask;
        } else {
            // resume task
            uRet = TaskResume(*pTask);
        }

        {
            UINT32 Rval = 0;
            const AMBA_KAL_TASK_t *Src = *pTask;
            AmbaMisra_TypeCast(&Rval,&Src);
            AmbaPrint_PrintUInt5("TaskCreate VBus detection: return 0x%x, Task 0x%X", uRet, Rval, 0, 0, 0);
        }
        break;
    case 1:
        if (*pTask == NULL) {
            uRet = TaskCreate(&UsbdCtrlTask,
                              UsbdCtrlTaskName,
                              12,
                              TaskEntry,
                              NULL,
                              CtrlTaskStackMem,
                              AMBAUSB_TEST_CTRL_TASK_STACK_SIZE,
                              1U, //AMBA_KAL_AUTO_START
                              0x01);
            *pTask = &UsbdCtrlTask;
        } else {
            // resume task
            uRet = TaskResume(*pTask);
        }

        {
            UINT32 Rval = 0;
            const AMBA_KAL_TASK_t *Src = *pTask;
            AmbaMisra_TypeCast(&Rval,&Src);
            AmbaPrint_PrintUInt5("TaskCreate Ctrl: return 0x%x, Task 0x%X", uRet, Rval, 0, 0, 0);
        }
        break;
    case 2:
        if (*pTask == NULL) {
            uRet = TaskCreate(&UsbdIsrTask,
                              UsbdIsrTaskName,
                              10,
                              TaskEntry,
                              NULL,
                              IsrTaskStackMem,
                              AMBAUSB_TEST_ISR_TASK_STACK_SIZE,
                              1U, //AMBA_KAL_AUTO_START
                              0x01);
            *pTask = &UsbdIsrTask;
        } else {
            // resume task
            uRet = TaskResume(*pTask);
        }

        {
            UINT32 Rval = 0;
            const AMBA_KAL_TASK_t *Src = *pTask;
            AmbaMisra_TypeCast(&Rval,&Src);
            AmbaPrint_PrintUInt5("TaskCreate Isr: return 0x%X, Task 0x%X", uRet, Rval, 0, 0, 0);
        }
        break;
    default:
        uRet = USB_ERR_FAIL;
        break;
    }
    return uRet;
}

static UINT32 USB_SystemTaskSuspendCb(UINT32 Code, USB_TASK_ENTRY_f TaskEntry, AMBA_KAL_TASK_t *pTask)
{
    UINT32  uRet = 0;
    void*   pTmp;

    AmbaMisra_TypeCast(&pTmp, &TaskEntry);

    AmbaMisra_TouchUnused(&Code);
    AmbaMisra_TouchUnused(pTmp);

    switch (Code) {
    case 0:
        uRet = TaskSuspend(pTask);
        break;
    case 1:
        uRet = TaskSuspend(pTask);
        break;
    case 2:
        uRet = TaskSuspend(pTask);
        break;
    default:
        uRet = USB_ERR_FAIL;
        break;
    }

    {
        UINT32 Rval = 0;
        AmbaMisra_TypeCast(&Rval,&pTask);
        AmbaPrint_PrintUInt5("TaskSuspend Task 0x%X return 0x%X for %d", Rval, uRet, Code, 0, 0);
    }
    return uRet;
}

static UINT32 HostEventCallback(UINT32 Code, UINT32 SubCode)
{
    AmbaMisra_TouchUnused(&Code);
    AmbaMisra_TouchUnused(&SubCode);

    switch (Code) {
    case USBH_EVENT_DEVICE_NO_RESPONSE:
        AmbaPrint_PrintUInt5("[USBH] Device No Respond", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_NOT_SUPPORT:
        AmbaPrint_PrintUInt5("[USBH] Device Not Support", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_OVER_CURRENT:
        AmbaPrint_PrintUInt5("[USBH] Device Over Current", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_HUB_NOT_SUPPORT:
        AmbaPrint_PrintUInt5("[USBH] Hub Not Support", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_INSERTED:
        AmbaPrint_PrintUInt5("[USBH] Device Inserted", 0, 0, 0, 0, 0);
        break;
    case USBH_EVENT_DEVICE_REMOVED:
        AmbaPrint_PrintUInt5("[USBH] Device Removed", 0, 0, 0, 0, 0);
        break;
    default:
        AmbaPrint_PrintUInt5("[USBH] Unknown Event: 0x%X", Code, 0, 0, 0, 0);
        break;
    }
    ;
    return USB_ERR_SUCCESS;
}

/**
* configure memory of usb protocol stack
* @param [in] pCaMem pointer of cache memory
* @param [in] CaSize size of cache memory
* @param [in] pNcMem pointer of non-cache memory
* @param [in] NcSize size of non-cache memory
* @return none
*/
void AmbaSvcUsb_MemInit(UINT8 *pCaMem, UINT32 CaSize, UINT8 *pNcMem, UINT32 NcSize)
{
    if (CaSize != 0U) {
        g_UsbCfg.pCaMem = pCaMem;
        g_UsbCfg.CaSize = CaSize;
    }

    if (NcSize != 0U) {
        g_UsbCfg.pNcMem = pNcMem;
        g_UsbCfg.NcSize = NcSize;
    }
}

/**
* configure memory of usb protocol stack
* @return none
*/
UINT32 AmbaSvcUsb_MemCheck(void)
{
    UINT32  Rval = 1U;

    if ((g_UsbCfg.CaSize != 0U) && (g_UsbCfg.NcSize != 0U)) {
        Rval = OK;
    } else {
        AmbaPrint_PrintUInt5("", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("## usb stack memory isn't configured,", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("##   please call AmbaSvcUsb_MemInit(...) to configure.", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("##     cache:     AMBA_USB_STACK_CA_SIZE", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("##     non-cache: AMBA_USB_STACK_NC_SIZE", 0U, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* initialization of usb device system
* @return 0-OK, 1-NG
*/
UINT32 AmbaSvcUsb_DeviceSysInit(void)
{
    UINT32             Rval = 1U;
    USBD_SYS_CONFIG_s  SysConfig;

    Rval = AmbaSvcUsb_MemCheck();
    if (Rval == OK) {
        if (AppDeviceInited == 0U) {
            AmbaSvcWrap_MisraMemset(&SysConfig, 0, sizeof(USBD_SYS_CONFIG_s));
            AmbaSvcWrap_MisraMemset(&VBusDetTask, 0, sizeof(AMBA_KAL_TASK_t));

            // USBX device won't use non-cache memory pool.
            // However we need to assign it.
            SysConfig.StackCacheMemory        = g_UsbCfg.pCaMem;
            SysConfig.StackCacheMemorySize    = g_UsbCfg.CaSize;
            SysConfig.StackNonCacheMemory     = g_UsbCfg.pNcMem;
            SysConfig.StackNonCacheMemorySize = g_UsbCfg.NcSize;
            SysConfig.CoreMask                = AMBA_KAL_CPU_CORE_MASK;
            SysConfig.SystemEventCallback     = USB_SystemEventCallback;
            SysConfig.SystemTaskCreateCb      = USB_SystemTaskCreateCb;
            SysConfig.SystemTaskSuspendCb     = USB_SystemTaskSuspendCb;
            SysConfig.InitSpeed               = USB_CONNECT_SPEED_HIGH;
            SysConfig.SetCfgThd               = 1000;

            Rval = AmbaUSBD_SystemInit(&SysConfig);
            if (Rval == OK) {
                AppDeviceInited = 1;
            }
        }
    }
    AmbaPrint_PrintUInt5("AmbaUSBD_System_Init: return 0x%x", Rval, 0, 0, 0, 0);

    return Rval;
}

/**
* de-initialization of usb device system
* @return none
*/
void AmbaSvcUsb_DeviceSysDeInit(void)
{
    UINT32 Rval;

    Rval = AmbaUSBD_SystemDeInit();
    AppDeviceInited = 0U;
    AmbaPrint_PrintUInt5("Brute release result = 0x%x", Rval, 0, 0, 0, 0);
}

/**
* class configuration of usb device
* @param [in] ClassID id of usb class
* @return none
*/
void AmbaSvcUsb_DeviceClassSet(UINT32 ClassID)
{
    AppUsbd_CurrentClassID = ClassID;

    /* Rule 8.9 */
    AmbaMisra_TouchUnused(LangID);
    AmbaMisra_TouchUnused(MtpDescFs);
    AmbaMisra_TouchUnused(MtpDescHs);
    AmbaMisra_TouchUnused(MtpStr);
}

/**
* class start of usb device
* @return none
*/
void AmbaSvcUsb_DeviceClassStart(void)
{
    static USBD_DESC_CUSTOM_INFO_s MtpDescFramework = {
        .DescFrameworkFs = MtpDescFs,
        .DescFrameworkHs = MtpDescHs,
#if defined(SVC_USB_SUPER_SPEED)
        .DescFrameworkSs = MtpDescSs,
#endif
        .StrFramework    = MtpStr,
        .LangIDFramework = LangID,
        .DescSizeFs      = (UINT32)sizeof(MtpDescFs),
        .DescSizeHs      = (UINT32)sizeof(MtpDescHs),
#if defined(SVC_USB_SUPER_SPEED)
        .DescSizeSs      = (UINT32)sizeof(MtpDescSs),
#endif
        .StrSize         = (UINT32)sizeof(MtpStr),
        .LangIDSize      = (UINT32)sizeof(LangID),
    };

    USBD_CLASS_CONFIG_s ClassConfig = {
        USBD_CLASS_NONE,
        NULL,
        NULL,
        NULL
    };

    UINT32 Rval;


    if (AppUsbd_CurrentClassID == AMBA_USBD_CLASS_MSC) {
        ClassConfig.ClassID   = USBD_CLASS_MSC;
        ClassConfig.Framework = &MscDescFramework;
        ClassConfig.ClassStartCb = AmbaUSBD_MSCClassStart;
        ClassConfig.ClassStopCb  = AmbaUSBD_MSCClassStop;
    } else if (AppUsbd_CurrentClassID == AMBA_USBD_CLASS_MTP) {
        ClassConfig.ClassID   = USBD_CLASS_MTP;
        ClassConfig.Framework = &MtpDescFramework;
        ClassConfig.ClassStartCb = AmbaUSBD_MTPClassStart;
        ClassConfig.ClassStopCb  = AmbaUSBD_MTPClassStop;
    }
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    else if (AppUsbd_CurrentClassID == AMBA_USBD_CLASS_UVC) {
        ClassConfig.ClassID   = USBD_CLASS_UVC;
        ClassConfig.Framework = &UvcDescFramework;
        ClassConfig.ClassStartCb = AmbaUSBD_UVCClassStart;
        ClassConfig.ClassStopCb  = AmbaUSBD_UVCClassStop;
    }
#endif
    else {
        AmbaPrint_PrintUInt5("Unknown class", 0, 0, 0, 0, 0);
    }

    Rval = AmbaUSBD_SystemClassStart(&ClassConfig);

    AmbaPrint_PrintUInt5("Class start result = 0x%x", Rval, 0, 0, 0, 0);
}

/**
* class stop of usb device
* @return none
*/
void AmbaSvcUsb_DeviceClassStop(void)
{
    UINT32 Rval;

    Rval = AmbaUSBD_SystemClassStop();
    AmbaPrint_PrintUInt5("Class stop result = 0x%x", Rval, 0, 0, 0, 0);
}

/**
* class start of usb host
* @param [in] pHost info block of usb host
* @return none
*/
void AmbaSvcUsb_HostClassStart(const AMBA_SVC_USBH_s *pHost)
{
    static UINT32 host_port_power_pin[1] = {SVC_USB_HOST_PWR};
    static UINT32 host_port_oc_pin[1] = {SVC_USB_HOST_OC};

    UINT32               i, Rval = 1U;
    USBH_CLASS_CONFIG_s  ClassConfig;

    AmbaMisra_TouchUnused(host_port_power_pin);
    AmbaMisra_TouchUnused(host_port_oc_pin);

    Rval = AmbaSvcUsb_MemCheck();
    if (Rval == OK) {
        // Configure GPIO
        for (i = 0; i < 1U; i++) {
            // Enable the EHCI over-current circuit.
            Rval = AmbaGPIO_SetFuncAlt(host_port_power_pin[i]);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("## fail to configure host_port_power_pin", 0U, 0U, 0U, 0U, 0U);
            }

            // Config the GPIO as USB over current detection.
            Rval = AmbaGPIO_SetFuncAlt(host_port_oc_pin[i]);
            if (Rval != OK) {
                AmbaPrint_PrintUInt5("## fail to configure host_port_oc_pin", 0U, 0U, 0U, 0U, 0U);
            }
        }

        // Configure parameters.
        AmbaSvcWrap_MisraMemset(&ClassConfig, 0, sizeof(USBH_CLASS_CONFIG_s));

        ClassConfig.ClassID                  = USBH_CLASS_STORAGE;
        ClassConfig.StackCacheMemory         = g_UsbCfg.pCaMem;
        ClassConfig.StackCacheMemorySize     = g_UsbCfg.CaSize;
        ClassConfig.StackNonCacheMemory      = g_UsbCfg.pNcMem;
        ClassConfig.StackNonCacheMemorySize  = g_UsbCfg.NcSize;
        ClassConfig.CoreMask                 = AMBA_KAL_CPU_CORE_MASK;
        ClassConfig.EnumTskInfo.Priority     = 70;
        ClassConfig.EnumTskInfo.AffinityMask = 0x01;
        ClassConfig.EnumTskInfo.StackSize    = 8 * 1024;
        ClassConfig.HcdTskInfo.Priority      = 70;
        ClassConfig.HcdTskInfo.AffinityMask  = 0x01;
        ClassConfig.HcdTskInfo.StackSize     = 16 * 1024;
        if (NULL == pHost->pfnCallback) {
            ClassConfig.SystemEventCallback = HostEventCallback;
        } else {
            ClassConfig.SystemEventCallback = pHost->pfnCallback;
        }
        ClassConfig.SelectHcd                = pHost->SelectHcd;
        ClassConfig.EhciOcPolarity           = pHost->OCPolarity;
        ClassConfig.Phy0Owner                = pHost->Phy0Owner;
        ClassConfig.ClassStartCallback       = AmbaUSBH_StorageStart;
        ClassConfig.ClassStopCallback        = AmbaUSBH_StorageStop;

        Rval = AmbaUSBH_SystemClassStart(&ClassConfig);
    }

    AmbaPrint_PrintUInt5("AmbaUSBH_System_ClassStart: return 0x%x", Rval, 0, 0, 0, 0);
}

/**
* class stop of usb host
* @return none
*/
void AmbaSvcUsb_HostClassStop(void)
{
    UINT32 Rval = AmbaUSBH_SystemClassStop();

    AmbaPrint_PrintUInt5("AmbaSvcUsb_HostClassStop, result = 0x%x", Rval, 0, 0, 0, 0);
}
