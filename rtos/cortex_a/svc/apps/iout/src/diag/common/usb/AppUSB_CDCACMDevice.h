/**
 *  @file AppUSB_CDCACMDevice.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details USB CDC-ACM Device API header file.
 */

#ifndef AMBA_USB_CDC_ACMD_H
#define AMBA_USB_CDC_ACMD_H

#ifndef AMBAUSB_DEVICE_API_H
#include "AmbaUSBD_API.h"
#endif
#ifndef STD_USB_H
#include "StdUSB.h"
#endif

typedef void (*CdcAcmNotify)(INT32 IsRunning);

extern USBD_DESC_CUSTOM_INFO_s AppUsbd_CdcAcmDescFramework;
extern UINT32 AppCdcAcmd_Start(void);
extern UINT32 AppCdcAcmd_Stop(void);
extern UINT32 AppCdcAcmd_NotificationRegister(CdcAcmNotify CallBackFunc);
extern void AppCdcAcmd_TestSend(UINT32 Timeout);
extern void AppCdcAcmd_TestReceive(UINT32 Timeout);

#endif

