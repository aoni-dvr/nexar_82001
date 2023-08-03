/**
 *  @file AmbaUSBD_Uvc.h
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
 *  @details USB Device Video Calss functions header file.
 */

#ifndef AMBAUSBD_UVC_H
#define AMBAUSBD_UVC_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifndef AMBAUSB_DEVICE_API_H
#include <AmbaUSBD_API.h>
#endif

#include <usbx/ux_api.h>
#include <usbx/ux_device_stack.h>
#include "ux_device_class_uvc.h"

// =======================================================
// TERM definition
//    PU: Processing Unit
//    IT: Input Terminal
// =======================================================

typedef struct {
    UINT32 Param1;
} UX_SLAVE_CLASS_UVC_PARAMETER;

// UVC structures

typedef UINT32 (*UvcdComplFn) (void *UserSpecific, UINT32 ActualLength, UINT32 Status);

// brightness is 16-bit signed value
typedef struct {
    INT16 Maximum;
    INT16 Minimum;
    INT16 Default;
    INT16 Current;
} USBD_UVC_BRIGHTNESS_s;

// contrast is 16-bit unsigned value
typedef struct {
    UINT16 Maximum;
    UINT16 Minimum;
    UINT16 Default;
    UINT16 Current;
} USBD_UVC_CONTRAST_s;

// saturation is 16-bit unsigned value
typedef struct {
    UINT16 Maximum;
    UINT16 Minimum;
    UINT16 Default;
    UINT16 Current;
} USBD_UVC_SATURATION_s;

// heu is 16-bit unsigned value
typedef struct {
    INT16 Maximum;
    INT16 Minimum;
    INT16 Default;
    INT16 Current;
} USBD_UVC_HUE_s;

// sharpness is 16-bit unsigned value
typedef struct {
    UINT16 Maximum;
    UINT16 Minimum;
    UINT16 Default;
    UINT16 Current;
} USBD_UVC_SHARPNESS_s;

// powerline is 8-bit unsigned value
typedef struct {
    UINT8 Maximum;
    UINT8 Minimum;
    UINT8 Default;
    UINT8 Current;
} USBD_UVC_POWER_LINE_s;

typedef struct {
    INT32 MaxBlue;
    INT32 MaxRed;
    INT32 MinBlue;
    INT32 MinRed;
    INT32 DefBlue;
    INT32 DefRed;
    INT32 CurBlue;
    INT32 CurRed;
} USBD_UVC_PU_WHITE_BALANCE_COMPONENT_SETTING_s;

typedef struct {
    INT32 MaxPan;
    INT32 MaxTilt;
    INT32 MinPan;
    INT32 MinTilt;
    INT32 DefPan;
    INT32 DefTilt;
    INT32 CurPan;
    INT32 CurTilt;
} USBD_UVC_IT_PANTILT_SETTING_s;

typedef struct {
    UINT32 (*EncodeStart)(UINT32 Index);
    UINT32 (*EncodeStop)(UINT32 Index);
    UINT32 (*PuSet)(UINT32 Index);
    UINT32 (*ItSet)(UINT32 Index);
    UINT32 (*XuHandler)(UINT32 Selector, UINT8 *Buffer, UINT32 Size, UINT32 Length);
} USBD_UVC_EVENT_CALLBACK_s;

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT32 MjpegQuality;
    UINT32 Framerate;
    UINT32 Format;
} USBD_UVC_HOST_CONFIG_s;


UINT32 AmbaUSBD_UVCClassStart(const USBD_DESC_CUSTOM_INFO_s *DescInfo);
UINT32 AmbaUSBD_UVCClassStop(void);
UINT32 usbd_uvc_desc_patch(UINT8 *whole_desc, UINT32 desc_size, UINT32 it_attribute, UINT32 pu_attribute);

UINT32 AmbaUSBD_UVCBulkSend(UINT32 Id, UINT8 *Buffer, UINT32 Size, UINT32 Timeout, UINT32 NoCopy);
UINT32 AmbaUSBD_UVCGetProbeMaxPayloadXferSize(void);
VOID   AmbaUSBD_UVC_SetProbeMaxPayloadXferSize(UINT32 Size);
UINT32 AmbaUSBD_UVCRegisterCallback(const USBD_UVC_EVENT_CALLBACK_s *cbs);

UINT32 AmbaUSBD_UVCSetITAttribute(UINT32 Attribute);
UINT32 AmbaUSBD_UVCSetPUAttribute(UINT32 Attribute);

UINT32 AmbaUSBD_UVCGetITAttribute(UINT32 *pAttribute);
UINT32 AmbaUSBD_UVCGetPUAttribute(UINT32 *pAttribute);

UINT32 AmbaUSBD_UVCGetHostConfig(USBD_UVC_HOST_CONFIG_s *pConfig);

UINT32 AmbaUSBD_UVCGetBrightness(USBD_UVC_BRIGHTNESS_s *pBrightness);
UINT32 AmbaUSBD_UVCSetBrightness(const USBD_UVC_BRIGHTNESS_s *pBrightness);
UINT32 AmbaUSBD_UVCGetContrast(USBD_UVC_CONTRAST_s *pContrast);
UINT32 AmbaUSBD_UVCSetContrast(const USBD_UVC_CONTRAST_s *pContrast);
UINT32 AmbaUSBD_UVCGetSaturation(USBD_UVC_SATURATION_s *pSaturation);
UINT32 AmbaUSBD_UVCSetSaturation(const USBD_UVC_SATURATION_s *pSaturation);
UINT32 AmbaUSBD_UVCGetHue(USBD_UVC_HUE_s *pHue);
UINT32 AmbaUSBD_UVCSetHue(const USBD_UVC_HUE_s *pHue);
UINT32 AmbaUSBD_UVCGetSharpness(USBD_UVC_SHARPNESS_s *pSharpness);
UINT32 AmbaUSBD_UVCSetSharpness(const USBD_UVC_SHARPNESS_s *pSharpness);
UINT32 AmbaUSBD_UVCGetPowerLine(USBD_UVC_POWER_LINE_s *pPowerLine);
UINT32 AmbaUSBD_UVCSetPowerLine(const USBD_UVC_POWER_LINE_s *pPowerLine);

//extern void  AmbaUSB_Utility_Print(const char *Str);
//extern void  AmbaUSB_Utility_PrintUInt5(const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

#endif

