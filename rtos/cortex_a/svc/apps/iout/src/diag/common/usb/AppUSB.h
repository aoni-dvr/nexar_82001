/**
 *  @file AppUSB.h
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
 *  @details USB application API header file.
 */


#ifndef APP_USB_H
#define APP_USB_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif
#define AMBA_KAL_SOURCE_CODE
#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include <AmbaIOUTDiag.h>
#include <AmbaUSB_Generic.h>
#include "AppUSB_MSCDevice.h"
#include "AppUSB_MTPDevice.h"
#include "AppUSB_CDCACMDevice.h"
#include "AppUSB_Util.h"
#ifndef STD_USB_H
#include "StdUSB.h"
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AppUSB_CDCMSCDevice.h"
#include "AppUSB_CDCMTPDevice.h"
#include "AppUSB_CDCACMMultiDevice.h"
#endif

#define USBX_CACHE_MEM_SIZE        (800 * 1024)
//#define USBX_NONCACHE_MEM_SIZE     (800 * 1024)

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define APP_USB_SUPPORT_SUPER_SPEED 1
#else
#define APP_USB_SUPPORT_SUPER_SPEED 0
#endif

#if (APP_USB_SUPPORT_SUPER_SPEED == 1)
#define SPEC_VERSION_FS         0x00, 0x02 // 0x0200
#define SPEC_VERSION_HS         0x10, 0x02 // 0x0210
#define SPEC_VERSION_SS         0x20, 0x03 // 0x0320
#else
#define SPEC_VERSION_FS         0x00, 0x02 // 0x0200
#define SPEC_VERSION_HS         0x00, 0x02 // 0x0200
#define SPEC_VERSION_SS         0x00, 0x02 // 0x0200
#endif

#define APP_USB_SSP_BOS_DESC_LENGTH 0x2A, 0x00 // (USB_DESC_LENGTH_BOS + USB_DESC_LENGTH_CAP_USB20EX + USB_DESC_LENGTH_CAP_SS_USB + USB_DESC_LENGTH_CAP_SSP_USB)

extern UINT8  g_usbx_memory_cached[USBX_CACHE_MEM_SIZE];

typedef enum {
    APPUSBD_CLASS_NONE = 0,
    APPUSBD_CLASS_MSC,      // mass storage class
    APPUSBD_CLASS_MTP,      // ptp class
    APPUSBD_CLASS_CDC_ACM,  // CDC-ACM class with single instances
    APPUSBD_CLASS_UVC,
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    APPUSBD_CLASS_CDC_ACM_MULTI,
    APPUSBD_CLASS_CDC_MSC,
    APPUSBD_CLASS_CDC_MTP,
#endif
    APPUSBD_CLASS_NUM
} APPUSBD_CLASS_e;

typedef UINT32 (*APPUSBD_CLR_STALL_CB)(UINT32 LogEPId);
typedef UINT32 (*APPUSBD_SET_INTERFACE_CB)(UINT32 InterfaceNUM, UINT32 AlternateSetting);
UINT32  AppUsbd_ClearStallCBRegister(APPUSBD_CLR_STALL_CB CB);
UINT32  AppUsbd_SetInterfaceCBRegister(UINT32 InterfaceId, APPUSBD_SET_INTERFACE_CB CB);

UINT32 AppUsbd_SysInit(VOID);
VOID   AppUsbd_ClassStart(VOID);
VOID   AppUsbd_ClassStop(VOID);
VOID   AppUsbd_BruteRelease(VOID);
void   AppUsbd_ChargerDet(UINT32 T1, UINT32 T2);
void   AppUsbd_VbusStatusPrint(void);
void   AppUsbd_EnumSpeedPrint(void);
void   AppUsbd_IsConfiguredPrint(void);
void   AppUsbd_VersionPrint(void);
VOID   AppUsbd_CurrentClassIDSet(APPUSBD_CLASS_e ClassID);
APPUSBD_CLASS_e AppUsbd_CurrentClassIDGet(void);

void   AppMsch_Start(UINT8 SelectHcd, UINT8 OCPolarity, UINT8 Phy0Owner);
void   AppMsch_FileCopyTest(const char *pFilePathSource, const char *pFilePathDestin);
void   AppUsbh_Stop(void);
void   AppUsbh_GpioConfig(void);
void   AppUsbh_TestModeSet(UINT32 TestMode);

void   AppUsb_DebugLevelSet(UINT32 Level);
UINT32 AppUsb_DebugLevelGet(void);

#endif

