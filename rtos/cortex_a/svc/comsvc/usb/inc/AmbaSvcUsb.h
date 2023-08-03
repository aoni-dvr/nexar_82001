/**
 *  @file AmbaSvcUsb.h
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
 *  @details USB general API header file.
 *
 */

#ifndef AMBA_SVC_USB_H
#define AMBA_SVC_USB_H

#include <AmbaTypes.h>
#define AMBA_KAL_SOURCE_CODE
#include "AmbaUSBD_API.h"
#include "AmbaUSBH_API.h"
#include "StdUSB.h"
#include "AmbaWrap.h"
#include <AmbaFS.h>

/* usb protocol stack memory */
#define AMBA_USB_STACK_CA_SIZE      (800U * 1024U)
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define AMBA_USB_STACK_NC_SIZE      (2U * 1024U * 1024U)
#define AMBA_USB_HOST_HCD           (USBH_HCD_XHCI) /* CV5 supports XHCI only in host mode */
#else
#define AMBA_USB_STACK_NC_SIZE      (800U * 1024U)
#define AMBA_USB_HOST_HCD           (USBH_HCD_EHCI)
#endif

/* usb device */
#define AMBA_USBD_CLASS_NONE        (0U)
#define AMBA_USBD_CLASS_MSC         (1U)
#define AMBA_USBD_CLASS_MTP         (2U)
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define AMBA_USBD_CLASS_UVC       (3U)
#define AMBA_USBD_CLASS_NUM       (4U)
#else
#define AMBA_USBD_CLASS_NUM       (3U)
#endif

/* usb host */
typedef struct {
    UINT8                   SelectHcd;
    UINT8                   OCPolarity;
    UINT8                   Phy0Owner;
    USBH_SYSTEM_EVENT_CB_f  pfnCallback;    /* CB hook function */
} AMBA_SVC_USBH_s;

void   AmbaSvcUsb_MemInit(UINT8 *pCaMem, UINT32 CaSize, UINT8 *pNcMem, UINT32 NcSize);
UINT32 AmbaSvcUsb_MemCheck(void);

UINT32 AmbaSvcUsb_DeviceSysInit(void);
void   AmbaSvcUsb_DeviceSysDeInit(void);
void   AmbaSvcUsb_DeviceClassSet(UINT32 ClassID);
void   AmbaSvcUsb_DeviceClassStart(void);
void   AmbaSvcUsb_DeviceClassStop(void);
void   AmbaSvcUsb_DeviceClassMscLoad(char StgDrive);
void   AmbaSvcUsb_DeviceClassMscUnload(char StgDrive);
UINT32 AmbaSvcUsb_DeviceClassMtpInit(char StgDrive);

void   AmbaSvcUsb_HostClassStart(const AMBA_SVC_USBH_s *pHost);
void   AmbaSvcUsb_HostClassStop(void);

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 AmbaSvcUsb_SetConfigured(int value);
int AmbaSvcUsb_GetConfigured(UINT32 *value);
#endif

#endif /* AMBA_SVC_USB_H */
