/**
 *  @file AppUSB_CDCACMMultiDevice.h
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details USB Device CDC-ACM Multi-Instance Calss functions header file.
 */

#ifndef APP_USB_CDC_ACM_MULTI_DEVICE_H
#define APP_USB_CDC_ACM_MULTI_DEVICE_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifndef AMBAUSB_DEVICE_API_H
#include <AmbaUSBD_API.h>
#endif

#include <usbx/ux_api.h>
#include <usbx/ux_device_stack.h>
#include <usbx/usbx_device_classes/ux_device_class_cdc_acm.h>

typedef void (*CdcAcmMultiNotify)(INT32 IsRunning);

extern USBD_DESC_CUSTOM_INFO_s g_cdc_acm_multi_desc_framework;

UINT32 AppCdcAcmMultid_ClassStart(const USBD_DESC_CUSTOM_INFO_s *pDescInfo);
UINT32 AppCdcAcmMultid_ClassStop(void);

UINT32 AppCdcAcmMultid_Read(UINT32 InstanceID, UCHAR *pBuffer, ULONG RequestLength, ULONG *pActualLength, ULONG Timeout);
UINT32 AppCdcAcmMultid_Write(UINT32 InstanceID, UCHAR *pBuffer, ULONG RequestLength, ULONG *pActualLength, ULONG Timeout);
UINT32 AppCdcAcmMultid_IsTerminalOpen(UINT32 InstanceID, UINT32 *pIsOpen);
UINT32 AppCdcAcmMultid_StackRegister(UINT32 ConfigurationIndex, UINT32 IntfaceIndex);

UINT32 AppCdcAcmMultid_Stop(void);
UINT32 AppCdcAcmMultid_Start(void);
UINT32 AppCdcAcmMultid_NotificationRegister(CdcAcmMultiNotify CallBackFunc);

#endif

