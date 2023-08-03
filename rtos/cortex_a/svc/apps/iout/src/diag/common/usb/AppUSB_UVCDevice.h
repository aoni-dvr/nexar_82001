/**
 *  @file AppUSB_UVCDevice.h
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
 *  @details USB Video Class appliation header file.
 */


#ifndef AMBA_USB_UVCD_H
#define AMBA_USB_UVCD_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif
#ifndef AMBAUSB_DEVICE_API_H
#include "AmbaUSBD_API.h"
#endif
#ifndef STD_USB_H
#include "StdUSB.h"
#endif
#ifndef STD_USB_VIDEO_CLASS_H
#include "StdUVC.h"
#endif
#ifndef AMBAUSBD_UVC_H
#include "device_classes/uvc/AmbaUSBD_Uvc.h"
#endif

#define UVC_IT_ID                         (0x01U)
#define UVC_PU_ID                         (0x05U)
#define UVC_OT1_ID                        (0x03U) // use for multiple stream
#define UVC_OT2_ID                        (0x06U) // use for multiple stream
#define UVC_EU_ID                         (0x04U)
#define UVC_XU_ID                         (0x0AU)
#define UVC_STREAM_NUMBER                 (0x02U)
#define UVC_INTERFACE_NUMBER              (UINT8)(UVC_STREAM_NUMBER + 0x01U)
#define UVC_VC_HEADER_DESC_LENGTH         (UINT8)(UVC_DESC_LENGTH_VC_INTF + UVC_STREAM_NUMBER - 1U)

extern USBD_DESC_CUSTOM_INFO_s g_uvc_desc_framework;
extern void AppUvcd_Start(void);
extern void AppUvcd_Stop(void);

#endif

