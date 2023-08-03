/**
 *  @file AppUSB_UVCDesc.c
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
 *  @details USB Device Video Class descriptors.
 */

#include <AmbaTypes.h>
#ifndef APP_USB_H
#include "AppUSB.h"
#endif
#ifndef AMBAUSB_DEVICE_API_H
#include "AmbaUSBD_API.h"
#endif
#ifndef STD_USB_VIDEO_CLASS_H
#include <StdUVC.h>
#endif

#ifndef AMBA_USB_UVCD_H
#include "AppUSB_UVCDevice.h"
#endif

#define UVCD_VENDOR_ID                    0x55, 0x42
#define UVCD_PRODUCT_ID                   0x5A, 0x01
#define USB_CONFIGURATION_NUMBER          1

#define UVC_XU_LEN                        (24 + UVC_XU_NUM_PIN + UVC_XU_CONTROL_SIZE)
#define UVC_XU_CONTROL_NUM                (0xA)
#define UVC_XU_CONTROL_MAP                0xFF, 0x03 // (0x03FF)
#define UVC_XU_CONTROL_SIZE               (0x02)
#define UVC_XU_NUM_PIN                    (0x01)

//#define UVC_HEADER_POOL_NUM     50
//#define UVC_HEADER_LEN          0x0C

#define UVC_BULK_MAX_PACKET_SIZE_SS       0x00, 0x04
#define UVC_BULK_MAX_PACKET_SIZE_HS       0x00, 0x02
#define UVC_BULK_MAX_PACKET_SIZE_FS       0x40, 0x00

#define UVC_BULK_IN_ENDPOINT1_ADDRESS     0x81
#define UVC_BULK_IN_ENDPOINT2_ADDRESS     0x83
#define UVC_INTURRUPT_IN_ENDPOINT_ADDRESS 0x82

#define UVC_IT_CONTROL_SIZE               0x03
#define UVC_SUPPORT_IT_CONTROLS           0x00, 0x00, 0x00 // 0x0000
#define UVC_PU_CONTROL_SIZE               0x02
#define UVC_SUPPORT_PU_CONTROLS           0x01, 0x00       // 0x0001

#if 0
#define UVC_SUPPORT_IT_CONTROLS           (UVC_CT_CONTROL_AUTO_EXPOSURE_MODE_MASK | \
                                           UVC_CT_CONTROL_EXPOSURE_TIME_ABSOLUTE_MASK | \
                                           UVC_CT_CONTROL_FOCUS_ABSOLUTE_MASK | \
                                           UVC_CT_CONTROL_FOCUS_AUTO_MASK | \
                                           UVC_CT_CONTROL_SCANNING_MODE_MASK | \
                                           UVC_CT_CONTROL_AUDO_EXPOSURE_PRIORITY_MASK | \
                                           UVC_CT_CONTROL_IRIS_ABSOLUTE_MASK | \
                                           UVC_CT_CONTROL_ZOOM_ABSOLUTE_MASK | \
                                           UVC_CT_CONTROL_PAN_TILE_ABSOLUTE_MASK | \
                                           UVC_CT_CONTROL_ROLL_ABSOLUTE_MASK | \
                                           UVC_CT_CONTROL_PRIVACY_MASK                  \
                                           )


#define UVC_SUPPORT_PU_CONTROLS           (UVC_PU_CONTROL_BRIGHTNESS_MASK | \
                                           UVC_PU_CONTROL_CONTRAST_MASK | \
                                           UVC_PU_CONTROL_HUE_AUTO_MASK | \
                                           UVC_PU_CONTROL_HUE_MASK | \
                                           UVC_PU_CONTROL_SATURATION_MASK | \
                                           UVC_PU_CONTROL_SHARPNESS_MASK | \
                                           UVC_PU_CONTROL_POWER_LINE_FREQUENCY_MASK | \
                                           UVC_PU_CONTROL_WB_TEMPERATURE_AUTO_MASK | \
                                           UVC_PU_CONTROL_WB_TEMPERATURE_MASK | \
                                           UVC_PU_CONTROL_BACKLIGHT_COMPENSATION_MASK | \
                                           UVC_PU_CONTROL_GAMMA_MASK | \
                                           UVC_PU_CONTROL_GAIN_MASK | \
                                           UVC_PU_CONTROL_DIGITAL_MULTIPLIER_MASK | \
                                           UVC_PU_CONTROL_DIGITAL_MULTIPLIERLIMIT_MASK  \
                                           )
#endif

#define UVC_MJPEG_MIN_BITRATE_848x480     0x00, 0x00, 0x77, 0x01 // (0x01770000)
#define UVC_MJPEG_MIN_BITRATE_1280x720    0x00, 0x00, 0x77, 0x01 // (0x01770000)
#define UVC_MJPEG_MIN_BITRATE_1920x1080   0x00, 0x00, 0x77, 0x02 // (0x02770000)

#define UVC_MJPEG_MAX_BITRATE_848x480     0x00, 0x00, 0xCA, 0x08 // (0x08CA0000)
#define UVC_MJPEG_MAX_BITRATE_1280x720    0x00, 0x00, 0xCA, 0x08 // (0x08CA0000)
#define UVC_MJPEG_MAX_BITRATE_1920x1080   0x00, 0x00, 0xCA, 0x10 // (0x10CA0000)

#define UVC_MJPEG_MAX_PAYLOAD_848x480     0x00, 0x00, 0x00, 0x01 // (0x01000000)
#define UVC_MJPEG_MAX_PAYLOAD_1280x720    0x00, 0x60, 0x09, 0x00 // (0x00096000)
#define UVC_MJPEG_MAX_PAYLOAD_1920x1080   0x00, 0x60, 0x09, 0x00 // (0x00096000)

#define UVC_FRAME_INTERVAL_30             0x15, 0x16, 0x05, 0x00 // (333333 = 0x51615)
#define UVC_FRAME_INTERVAL_25             0x80, 0x1A, 0x06, 0x00 // (400000 = 0x61A80)
#define UVC_FRAME_INTERVAL_20             0x20, 0xA1, 0x07, 0x00 // (500000 = 0x7A120)
#define UVC_FRAME_INTERVAL_15             0x2A, 0x2C, 0x0A, 0x00 // (666666 = 0xA2C2A)
#define UVC_FRAME_INTERVAL_10             0x40, 0x42, 0x0F, 0x00 // (1000000 = 0xF4240)
#define UVC_FRAME_INTERVAL_5              0x80, 0x84, 0x1E, 0x00 // (2000000 = 0x1E8480)

#define STILL_RES_WIDTH_6000   0x70, 0x17
#define STILL_RES_HEIGHT_4000  0xA0, 0x0F

// h264 guid = {34363248-0000-0010-8000-00AA00389B71}
#define UVC_H264_GUID                     0x48, 0x32, 0x36, 0x34, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71
//U32TOU8LE(0x34363248),
//U32TOU8LE(0x00100000),
//U32TOU8LE(0xAA000080),
//U32TOU8LE(0x719B3800),

#define UVC_XU_GUID 0x49, 0xE6, 0x07, 0xE3, 0x18, 0x46, 0xFF, 0xA3, 0x82, 0xFC, 0x2D, 0x8B, 0x5F, 0x21, 0x67, 0x73
//U32TOU8LE(0xE307E649),
//U32TOU8LE(0xA3FF4618),
//U32TOU8LE(0x8B2DFC82),
//U32TOU8LE(0x7367215F),

/*-----------------------------------------------------------------------------------------------*\
* Video Class 1.0 MJPEG + H264 Descriptor
\*-----------------------------------------------------------------------------------------------*/
static UINT8 usb_desc_full_speed[] __attribute__ ((aligned(32))) = {
    /* Device Descriptor*/
    USB_DESC_LENGTH_DEV,                  // Length
    USB_DESC_TYPE_DEV,                    // Type
    SPEC_VERSION_FS,                      // Version
    USB_DEVICE_CLASS_CODE_UVC,            // class code
    USB_DEVICE_SUBCLASS_CODE_UVC,         // subclass code
    USB_DEVICE_PROTOCOL_CODE_UVC,         // protocol code
    0x40,                                 // maximum packet size
    UVCD_VENDOR_ID,                       // VID
    UVCD_PRODUCT_ID,                      // PID
    0x00, 0x02,                           // Device release num
    0x01,                                 // iManufacture
    0x02,                                 // iProduct
    0x00,                                 // iSerialNumber
    USB_CONFIGURATION_NUMBER,

    USB_DESC_LENGTH_QUALIFIER,            // this descriptor size
    USB_DESC_TYPE_QUALIFIER,              // descriptor type
    SPEC_VERSION_FS,                      // Spec version
    USB_DEVICE_CLASS_CODE_UVC,            // class code
    USB_DEVICE_SUBCLASS_CODE_UVC,         // subclass code
    USB_DEVICE_PROTOCOL_CODE_UVC,         // procotol code
    0x40,                                 // max packet size
    USB_CONFIGURATION_NUMBER,             // configuration number
    0x00,                                 // reserved

    /* configuration descriptor */
    USB_DESC_LENGTH_CFG,                  // length
    USB_DESC_TYPE_CFG,                    // type
    0x00, 0x00,                           // total length, calcuated later
    0x02,                                 // number of interface
    0x01,                                 // config value
    0x00,                                 // config index
    0xC0,                                 // attribute
    0x32,                                 // max power unit=2mA

    /* IAD descriptor*/
    USB_DESC_LENGTH_IAD,                  // length
    USB_DESC_TYPE_IAD,                    // type
    0x00,                                 // bFirstInterface
    UVC_INTERFACE_NUMBER,                 // bInterfaceCount
    UVC_INTF_CLASS_CODE_VIDEO,            // bFunctionClass
    UVC_INTF_SUBCLASS_CODE_IAD,           // bFunctionSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,      // bFunctionProtocol
    0x02,                                 // iFunction

    // Standard Video Control Interface Descriptor
    USB_DESC_LENGTH_INTF,                 // length
    USB_DESC_TYPE_INTF,                   // type
    0x00,                                 // bInterfaceNumber
    0x00,                                 // bAlternateSetting
    0x01,                                 // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,            // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_CONTROL, // bFunctionSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,      // bFunctionProtocol
    0x02,                                 // iInterface

    // Class specific Video Control interface header descriptor
    UVC_VC_HEADER_DESC_LENGTH,            // Length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_HEADER,           // subtype
    0x00, 0x01,                           // Spec version
    0x00, 0x00,                           // total length, calcuated later
    0x00, 0x6C, 0xDC, 0x02,               // dwClockFrequency
    UVC_STREAM_NUMBER,                    // bInCollection
    0x01,                                 // baInterfaceNr(1)
    0x02,                                 // baInterfaceNr(2)

    // Video control output terminal 1 descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT1_ID,                           // bTerminalID
    0x01, 0x01,                           // wTerminalType, UVC_TERMINAL_TYPE_STREAMING = 0x0101
    0x00,                                 // bAssocTerminal
    UVC_XU_ID,                            // bSourceID
    0x00,                                 // iTerminal

    // Video control output terminal 1 descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT2_ID,                           // bTerminalID
    0x01, 0x01,                           // wTerminalType, UVC_TERMINAL_TYPE_STREAMING = 0x0101
    0x00,                                 // bAssocTerminal
    UVC_XU_ID,                            // bSourceID
    0x00,                                 // iTerminal

    // Video control eXtension Unit descriptor
    UVC_XU_LEN,                           // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT,   // subtype
    UVC_XU_ID,                            // bUnitID
    UVC_XU_GUID,
    UVC_XU_CONTROL_NUM,                   // bNumControls
    UVC_XU_NUM_PIN,                       // bNrInPins
    UVC_PU_ID,                            // baSourceID
    UVC_XU_CONTROL_SIZE,                  // bControlSize
    UVC_XU_CONTROL_MAP,                   // bmControls
    0x00,                                 // iExtension

    // Video Control Input Terminal descriptor
    UVC_DESC_LENGTH_INPUT_TERMINAL,       // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL,   // subtype
    UVC_IT_ID,                            // bTerminalID
    0x01, 0x02,                           // wTerminalType, UVC_INPUT_TERMINAL_TYPE_CAMERA = 0x0201
    0x00,                                 // bAssocTerminal
    0x00,                                 // iTerminal
    0x00, 0x00,                           // wObjectiveFocalLengthMin, no optical zoom is supported
    0x00, 0x00,                           // wObjectiveFocalLengthMax, no optical zoom is supported
    0x00, 0x00,                           // wOcularFocalLength
    UVC_IT_CONTROL_SIZE,                  // bControlSize
    UVC_SUPPORT_IT_CONTROLS,              // bmControls

    // Video Control Processing Unit descriptor
    UVC_DESC_LENGTH_PROCESSING_UNIT,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT,  // subtype
    UVC_PU_ID,                            // bUnitID
    UVC_IT_ID,                            // bSourceID
    0x00, 0x00,                           // wMaxMultiplier
    UVC_PU_CONTROL_SIZE,                  // bControlSize
    UVC_SUPPORT_PU_CONTROLS,              // bmControls
    0x00,                                 // iProcessing

    // Video Control Interrupt Endpoint Descriptor
    USB_DESC_LENGTH_ENDPOINT,             // length
    USB_DESC_TYPE_ENDPOINT,               // type
    UVC_INTURRUPT_IN_ENDPOINT_ADDRESS,    // address
    0x03,                                 // bmAttributes
    0x10, 0x00,                           // max packet size
    0x08,                                 // interval

    // class-specific VC Interrupt endpoint descriptor
    UVC_DESC_LENGTH_CS_ENDPOINT,          // length
    UVC_DESC_TYPE_CS_ENDPOINT,            // type
    UVC_ENDPOINT_DESC_SUBTYPE_INTERRUPT,  // subtype
    0x10, 0x00,                           // max packet size

    // Video Streaming Interface descriptor 0
    USB_DESC_LENGTH_INTF,
    USB_DESC_TYPE_INTF,
    0x01,                                   // bInterfaceNumber
    0x00,                                   // bAlternateSetting
    0x01,                                   // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,              // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_STREAMING, // bInterfaceSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,        // bInterfaceProtocol
    0x00,                                   // iInterface

    // class-specific VS video input header descriptor
    0x0F,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x02,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT1_ADDRESS,          // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT1_ID,                             // bTerminalLink
    0x00,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
    0x00,                                   // bmaControls - H264
    /*
       D0 - Key Frame Rate
       D1 - P Frame Rate
       D2 - Compression Quality
       D3 - Compression Window Size
       D4 - Generate Key Frame
       D5 - Update Frame Segment
       D6 - RSVD
       D7 - RSVD
     */

    // Video Streaming MJPEG format type descriptor
    UVC_DESC_LENGTH_VS_MJPEG_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    0x00,                                   // bmFlags
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming MJPEG frame type descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_848x480,          // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming Frame Based format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,  // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED, // subtype
    0x02,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    UVC_H264_GUID,
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming Frame Base Frame descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT1_ADDRESS,
    0x02,                        // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_FS, // max packet size
    0x00,                        // interval

    // Video Streaming Interface descriptor 1
    USB_DESC_LENGTH_INTF,
    USB_DESC_TYPE_INTF,
    0x02,                                   // bInterfaceNumber
    0x00,                                   // bAlternateSetting
    0x01,                                   // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,              // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_STREAMING, // bInterfaceSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,        // bInterfaceProtocol
    0x00,                                   // iInterface

    // class-specific VS video input header descriptor
    0x0F,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x02,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT2_ADDRESS,          // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT2_ID,                             // bTerminalLink
    0x00,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
    0x00,                                   // bmaControls - H264
    /*
       D0 - Key Frame Rate
       D1 - P Frame Rate
       D2 - Compression Quality
       D3 - Compression Window Size
       D4 - Generate Key Frame
       D5 - Update Frame Segment
       D6 - RSVD
       D7 - RSVD
     */

    // Video Streaming MJPEG format type descriptor
    UVC_DESC_LENGTH_VS_MJPEG_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    0x00,                                   // bmFlags
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming MJPEG frame type descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_848x480,          // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming Frame Based format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,  // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED, // subtype
    0x02,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    UVC_H264_GUID,
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming Frame Base Frame descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT2_ADDRESS,
    0x02,                        // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_FS, // max packet size
    0x00,                        // interval

};

static UINT8 usb_desc_high_speed[] __attribute__ ((aligned(32))) = {
    /* Device Descriptor*/
    USB_DESC_LENGTH_DEV,                  // Length
    USB_DESC_TYPE_DEV,                    // Type
    SPEC_VERSION_HS,                      // Version
    USB_DEVICE_CLASS_CODE_UVC,            // class code
    USB_DEVICE_SUBCLASS_CODE_UVC,         // subclass code
    USB_DEVICE_PROTOCOL_CODE_UVC,         // protocol code
    0x40,                                 // maximum packet size
    UVCD_VENDOR_ID,                       // VID
    UVCD_PRODUCT_ID,                      // PID
    0x00, 0x02,                           // Device release num
    0x01,                                 // iManufacture
    0x02,                                 // iProduct
    0x00,                                 // iserialNumber
    USB_CONFIGURATION_NUMBER,             // bNumConfiguations

    USB_DESC_LENGTH_QUALIFIER,            // this descriptor size
    USB_DESC_TYPE_QUALIFIER,              // descriptor type
    SPEC_VERSION_HS,                      // Spec version
    USB_DEVICE_CLASS_CODE_UVC,            // class code
    USB_DEVICE_SUBCLASS_CODE_UVC,         // subclass code
    USB_DEVICE_PROTOCOL_CODE_UVC,         // procotol code
    0x40,                                 // max packet size
    USB_CONFIGURATION_NUMBER,             // configuration number
    0x00,                                 // reserved

#if (APP_USB_SUPPORT_SUPER_SPEED == 1)

    /* Device BOS descriptor */
    USB_DESC_LENGTH_BOS,           // this descriptor size
    USB_DESC_TYPE_BOS,             // descriptor type
    BOS_DESCRIPTOR_LENGTH_SS, 0x0, // total length
    USB_DESC_CAP_NUM,              // number of capability.

    /* USB 2.0 extenstion descriptor */
    USB_DESC_LENGTH_CAP_USB20EX,   // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_USB20EX,     // capability type
    0x06, 0x00, 0x00, 0x00,        // attribute

    /* Super Speed USB descriptor */
    USB_DESC_LENGTH_CAP_SS_USB,    // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_SS_USB,      // capability type
    0x00,                          // attribute
    0x0E, 0x00,                    // SpeedSupport
    0x01,                          // FunctionSupport
    0x0A,                          // U1 Device Exit latency
    0x00, 0x01,                    // U2 Device Exit Latency

#endif

    /* configuration descriptor */
    USB_DESC_LENGTH_CFG,                  // length
    USB_DESC_TYPE_CFG,                    // type
    0x00, 0x00,                           // total length, calcuated later
    UVC_INTERFACE_NUMBER,                 // number of interface
    0x01,                                 // config value
    0x00,                                 // config index
    0xC0,                                 // attribute
    0x32,                                 // max power unit=2mA

    /* IAD descriptor*/
    USB_DESC_LENGTH_IAD,                  // length
    USB_DESC_TYPE_IAD,                    // type
    0x00,                                 // bFirstInterface
    UVC_INTERFACE_NUMBER,                 // bInterfaceCount
    UVC_INTF_CLASS_CODE_VIDEO,            // bFunctionClass
    UVC_INTF_SUBCLASS_CODE_IAD,           // bFunctionSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,      // bFunctionProtocol
    0x02,                                 // iFunction

    // Standard Video Control Interface Descriptor
    USB_DESC_LENGTH_INTF,                 // length
    USB_DESC_TYPE_INTF,                   // type
    0x00,                                 // bInterfaceNumber
    0x00,                                 // bAlternateSetting
    0x01,                                 // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,            // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_CONTROL, // bFunctionSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,      // bFunctionProtocol
    0x02,                                 // iInterface

    // Class specific Video Control interface header descriptor
    UVC_VC_HEADER_DESC_LENGTH,            // Length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_HEADER,           // subtype
    0x00, 0x01,                           // Spec version
    0x00, 0x00,                           // total length, calcuated later
    0x00, 0x6C, 0xDC, 0x02,               // dwClockFrequency
    UVC_STREAM_NUMBER,                    // bInCollection
    0x01,                                 // baInterfaceNr(1)
    0x02,                                 // baInterfaceNr(2)

    // Video control output terminal 1 descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT1_ID,                           // bTerminalID
    0x01, 0x01,                           // wTerminalType, UVC_TERMINAL_TYPE_STREAMING = 0x0101
    0x00,                                 // bAssocTerminal
    UVC_XU_ID,                            // bSourceID
    0x00,                                 // iTerminal

    // Video control output terminal 2 descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT2_ID,                           // bTerminalID
    0x01, 0x01,                           // wTerminalType, UVC_TERMINAL_TYPE_STREAMING = 0x0101
    0x00,                                 // bAssocTerminal
    UVC_XU_ID,                            // bSourceID
    0x00,                                 // iTerminal

    // Video control eXtension Unit descriptor
    UVC_XU_LEN,                           // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT,   // subtype
    UVC_XU_ID,                            // bUnitID
    UVC_XU_GUID,
    UVC_XU_CONTROL_NUM,                   // bNumControls
    UVC_XU_NUM_PIN,                       // bNrInPins
    UVC_PU_ID,                            // baSourceID
    UVC_XU_CONTROL_SIZE,                  // bControlSize
    UVC_XU_CONTROL_MAP,                   // bmControls
    0x00,                                 // iExtension

    // Video Control Input Terminal descriptor
    UVC_DESC_LENGTH_INPUT_TERMINAL,       // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL,   // subtype
    UVC_IT_ID,                            // bTerminalID
    0x01, 0x02,                           // wTerminalType, UVC_INPUT_TERMINAL_TYPE_CAMERA = 0x0201
    0x00,                                 // bAssocTerminal
    0x00,                                 // iTerminal
    0x00, 0x00,                           // wObjectiveFocalLengthMin, no optical zoom is supported
    0x00, 0x00,                           // wObjectiveFocalLengthMax, no optical zoom is supported
    0x00, 0x00,                           // wOcularFocalLength
    UVC_IT_CONTROL_SIZE,                  // bControlSize
    UVC_SUPPORT_IT_CONTROLS,              // bmControls

    // Video Control Processing Unit descriptor
    UVC_DESC_LENGTH_PROCESSING_UNIT,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT,  // subtype
    UVC_PU_ID,                            // bUnitID
    UVC_IT_ID,                            // bSourceID
    0x00, 0x00,                           // wMaxMultiplier
    UVC_PU_CONTROL_SIZE,                  // bControlSize
    UVC_SUPPORT_PU_CONTROLS,              // bmControls
    0x00,                                 // iProcessing

    // Video Control Interrupt Endpoint Descriptor
    USB_DESC_LENGTH_ENDPOINT,             // length
    USB_DESC_TYPE_ENDPOINT,               // type
    UVC_INTURRUPT_IN_ENDPOINT_ADDRESS,    // address
    0x03,                                 // bmAttributes
    0x10, 0x00,                           // max packet size
    0x08,                                 // interval

    // class-specific VC Interrupt endpoint descriptor
    UVC_DESC_LENGTH_CS_ENDPOINT,          // length
    UVC_DESC_TYPE_CS_ENDPOINT,            // type
    UVC_ENDPOINT_DESC_SUBTYPE_INTERRUPT,  // subtype
    0x10, 0x00,                           // max packet size

    // Video Streaming Interface descriptor 0
    USB_DESC_LENGTH_INTF,
    USB_DESC_TYPE_INTF,
    0x01,                                   // bInterfaceNumber
    0x00,                                   // bAlternateSetting
    0x01,                                   // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,              // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_STREAMING, // bInterfaceSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,        // bInterfaceProtocol
    0x00,                                   // iInterface

    // class-specific VS video input header descriptor
    0x0F,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x02,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT1_ADDRESS,          // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT1_ID,                             // bTerminalLink
    0x02,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
    0x00,                                   // bmaControls - H264
    /*
       D0 - Key Frame Rate
       D1 - P Frame Rate
       D2 - Compression Quality
       D3 - Compression Window Size
       D4 - Generate Key Frame
       D5 - Update Frame Segment
       D6 - RSVD
       D7 - RSVD
     */

    // Video Streaming MJPEG format type descriptor
    UVC_DESC_LENGTH_VS_MJPEG_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG,       // subtype
    0x01,                                   // bFormatIndex
    0x03,                                   // bNumFrameDescriptors
    0x00,                                   // bmFlags
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming MJPEG frame type descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_848x480,          // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming MJPEG frame type descriptor: index 2
    0x32,
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x02,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x00, 0x05,                             // bWidth, 1280 = 0x0500
    0xD0, 0x02,                             // bHeight 720 = 0x02D0
    UVC_MJPEG_MIN_BITRATE_1280x720,         // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1280x720,         // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_1280x720,         // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming MJPEG frame type descriptor: index 3
    0x32,
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x03,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_1920x1080,        // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Still Image frame descriptor
    0x0b,                                       // length
    UVC_DESC_TYPE_CS_INTF,                      // type
    UVC_DESC_SUBTYPE_VS_STILL_IMAGE_FRAME,      // subtype
    0x00,                                       // bEndpointAddress
    0x01,                                       // bNumImageSizePatterns
    STILL_RES_WIDTH_6000,                       // bWidth
    STILL_RES_HEIGHT_4000,                      // bHeight
    0x01,                                       // bNumCompressionPatterns
    0x00,                                       // bCompression

    // Video Streaming Frame Based format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,  // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED, // subtype
    0x02,                                   // bFormatIndex
    0x03,                                   // bNumFrameDescriptors
    UVC_H264_GUID,
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming Frame Base Frame descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming Frame Based Frame descriptor: index 2
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x02,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x00, 0x05,                             // bWidth, 1280 = 0x0500
    0xD0, 0x02,                             // bHeight 720 = 0x02D0
    UVC_MJPEG_MIN_BITRATE_1280x720,         // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1280x720,         // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]


    // Video Streaming Frame Based Frame descriptor: index 3
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x03,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT1_ADDRESS,
    0x02,                        // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_HS, // max packet size
    0x00,                        // interval

    // Video Streaming Interface descriptor 1
    USB_DESC_LENGTH_INTF,
    USB_DESC_TYPE_INTF,
    0x02,                                   // bInterfaceNumber
    0x00,                                   // bAlternateSetting
    0x01,                                   // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,              // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_STREAMING, // bInterfaceSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,        // bInterfaceProtocol
    0x00,                                   // iInterface

    // class-specific VS video input header descriptor
    0x0F,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x02,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT2_ADDRESS,          // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT2_ID,                             // bTerminalLink
    0x00,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
    0x00,                                   // bmaControls - H264
    /*
       D0 - Key Frame Rate
       D1 - P Frame Rate
       D2 - Compression Quality
       D3 - Compression Window Size
       D4 - Generate Key Frame
       D5 - Update Frame Segment
       D6 - RSVD
       D7 - RSVD
     */

    // Video Streaming MJPEG format type descriptor
    UVC_DESC_LENGTH_VS_MJPEG_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    0x00,                                   // bmFlags
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming MJPEG frame type descriptor: index 1
    0x32,
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_1920x1080,        // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming Frame Based format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,  // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED, // subtype
    0x02,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    UVC_H264_GUID,
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming Frame Based Frame descriptor: index 3
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT2_ADDRESS,
    0x02,                        // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_HS, // max packet size
    0x00,                        // interval

};

static UINT8 usb_desc_super_speed[] __attribute__ ((aligned(32))) = {
    /* Device Descriptor*/
    USB_DESC_LENGTH_DEV,                  // Length
    USB_DESC_TYPE_DEV,                    // Type
    SPEC_VERSION_SS,                      // Version
    USB_DEVICE_CLASS_CODE_UVC,            // class code
    USB_DEVICE_SUBCLASS_CODE_UVC,         // subclass code
    USB_DEVICE_PROTOCOL_CODE_UVC,         // protocol code
    0x09,                                 // maximum packet size
    UVCD_VENDOR_ID,                       // VID
    UVCD_PRODUCT_ID,                      // PID
    0x00, 0x02,                           // Device release num
    0x01,                                 // iManufacture
    0x02,                                 // iProduct
    0x00,                                 // iserialNumber
    USB_CONFIGURATION_NUMBER,             // bNumConfiguations

    /* Device BOS descriptor */
    USB_DESC_LENGTH_BOS,           // this descriptor size
    USB_DESC_TYPE_BOS,             // descriptor type
    APP_USB_SSP_BOS_DESC_LENGTH,   // total length
    0x03,                          // number of capability.

    /* USB 2.0 extenstion descriptor */
    USB_DESC_LENGTH_CAP_USB20EX,   // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_USB20EX,     // capability type
    0x06, 0x00, 0x00, 0x00,        // attribute

    /* Super Speed USB descriptor */
    USB_DESC_LENGTH_CAP_SS_USB,    // this descriptor size
    USB_DESC_TYPE_DEV_CAP,         // descriptor type
    USB_DESC_CAP_TYPE_SS_USB,      // capability type
    0x00,                          // attribute
    0x0E, 0x00,                    // SpeedSupport
    0x01,                          // FunctionSupport
    0x0A,                          // U1 Device Exit latency
    0x00, 0x01,                    // U2 Device Exit Latency

    /* Super Speed Plus USB Device Capability Descriptor */
    USB_DESC_LENGTH_CAP_SSP_USB,    // this descriptor size
    USB_DESC_TYPE_DEV_CAP,          // descriptor type
    USB_DESC_CAP_TYPE_SSP_USB,      // capability type
    0x00,                           // bReserved
    0x01, 0x00, 0x00, 0x00,         // attribute (4 bytes) SSAC = 1+1, SSIC = 0+1
    0x00, 0x11,                     // wFunctionalitySupport (2 bytes) SSID = 0, TX/RX lane count = 1
    0x00, 0x00,                     // wReserved
    0x30, 0x40, 0x0A, 0x00,         // bmSublinkSpeedAttr[0] SSID = 0, LSE = 3, ST = 0(RX), LP = 1, LSM = 10
    0xB0, 0x40, 0x0A, 0x00,         // bmSublinkSpeedAttr[1] SSID = 0, LSE = 3, ST = 2(TX), LP = 1, LSM = 10

    /* configuration descriptor */
    USB_DESC_LENGTH_CFG,                  // length
    USB_DESC_TYPE_CFG,                    // type
    0x00, 0x00,                           // total length, calcuated later
    UVC_INTERFACE_NUMBER,                 // number of interface
    0x01,                                 // config value
    0x00,                                 // config index
    0xC0,                                 // attribute
    0x32,                                 // max power unit=2mA

    /* IAD descriptor*/
    USB_DESC_LENGTH_IAD,                  // length
    USB_DESC_TYPE_IAD,                    // type
    0x00,                                 // bFirstInterface
    UVC_INTERFACE_NUMBER,                 // bInterfaceCount
    UVC_INTF_CLASS_CODE_VIDEO,            // bFunctionClass
    UVC_INTF_SUBCLASS_CODE_IAD,           // bFunctionSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,      // bFunctionProtocol
    0x02,                                 // iFunction

    // Standard Video Control Interface Descriptor
    USB_DESC_LENGTH_INTF,                 // length
    USB_DESC_TYPE_INTF,                   // type
    0x00,                                 // bInterfaceNumber
    0x00,                                 // bAlternateSetting
    0x01,                                 // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,            // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_CONTROL, // bFunctionSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,      // bFunctionProtocol
    0x02,                                 // iInterface

    // Class specific Video Control interface header descriptor
    UVC_VC_HEADER_DESC_LENGTH,            // Length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_HEADER,           // subtype
    0x00, 0x01,                           // Spec version
    0x00, 0x00,                           // total length, calcuated later
    0x00, 0x6C, 0xDC, 0x02,               // dwClockFrequency
    UVC_STREAM_NUMBER,                    // bInCollection
    0x01,                                 // baInterfaceNr(1)
    0x02,                                 // baInterfaceNr(2)

    // Video control output terminal 1 descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT1_ID,                           // bTerminalID
    0x01, 0x01,                           // wTerminalType, UVC_TERMINAL_TYPE_STREAMING = 0x0101
    0x00,                                 // bAssocTerminal
    UVC_XU_ID,                            // bSourceID
    0x00,                                 // iTerminal

    // Video control output terminal 2 descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT2_ID,                           // bTerminalID
    0x01, 0x01,                           // wTerminalType, UVC_TERMINAL_TYPE_STREAMING = 0x0101
    0x00,                                 // bAssocTerminal
    UVC_XU_ID,                            // bSourceID
    0x00,                                 // iTerminal

    // Video control eXtension Unit descriptor
    UVC_XU_LEN,                           // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT,   // subtype
    UVC_XU_ID,                            // bUnitID
    UVC_XU_GUID,
    UVC_XU_CONTROL_NUM,                   // bNumControls
    UVC_XU_NUM_PIN,                       // bNrInPins
    UVC_PU_ID,                            // baSourceID
    UVC_XU_CONTROL_SIZE,                  // bControlSize
    UVC_XU_CONTROL_MAP,                   // bmControls
    0x00,                                 // iExtension

    // Video Control Input Terminal descriptor
    UVC_DESC_LENGTH_INPUT_TERMINAL,       // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL,   // subtype
    UVC_IT_ID,                            // bTerminalID
    0x01, 0x02,                           // wTerminalType, UVC_INPUT_TERMINAL_TYPE_CAMERA = 0x0201
    0x00,                                 // bAssocTerminal
    0x00,                                 // iTerminal
    0x00, 0x00,                           // wObjectiveFocalLengthMin, no optical zoom is supported
    0x00, 0x00,                           // wObjectiveFocalLengthMax, no optical zoom is supported
    0x00, 0x00,                           // wOcularFocalLength
    UVC_IT_CONTROL_SIZE,                  // bControlSize
    UVC_SUPPORT_IT_CONTROLS,              // bmControls

    // Video Control Processing Unit descriptor
    UVC_DESC_LENGTH_PROCESSING_UNIT,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT,  // subtype
    UVC_PU_ID,                            // bUnitID
    UVC_IT_ID,                            // bSourceID
    0x00, 0x00,                           // wMaxMultiplier
    UVC_PU_CONTROL_SIZE,                  // bControlSize
    UVC_SUPPORT_PU_CONTROLS,              // bmControls
    0x00,                                 // iProcessing

    // Video Control Interrupt Endpoint Descriptor
    USB_DESC_LENGTH_ENDPOINT,             // length
    USB_DESC_TYPE_ENDPOINT,               // type
    UVC_INTURRUPT_IN_ENDPOINT_ADDRESS,    // address
    0x03,                                 // bmAttributes
    0x10, 0x00,                           // max packet size
    0x08,                                 // interval

    // Super Speed Endpoint Companion descriptor
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x00,                           // Maximum Burst
    0x00,                           // stream
    0x10, 0x00,                     // BytesPerInterval

    // class-specific VC Interrupt endpoint descriptor
    UVC_DESC_LENGTH_CS_ENDPOINT,          // length
    UVC_DESC_TYPE_CS_ENDPOINT,            // type
    UVC_ENDPOINT_DESC_SUBTYPE_INTERRUPT,  // subtype
    0x10, 0x00,                           // max packet size

    // Video Streaming Interface descriptor 0
    USB_DESC_LENGTH_INTF,
    USB_DESC_TYPE_INTF,
    0x01,                                   // bInterfaceNumber
    0x00,                                   // bAlternateSetting
    0x01,                                   // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,              // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_STREAMING, // bInterfaceSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,        // bInterfaceProtocol
    0x00,                                   // iInterface

    // class-specific VS video input header descriptor
    0x0F,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x02,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT1_ADDRESS,          // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT1_ID,                             // bTerminalLink
    0x02,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
    0x00,                                   // bmaControls - H264
    /*
       D0 - Key Frame Rate
       D1 - P Frame Rate
       D2 - Compression Quality
       D3 - Compression Window Size
       D4 - Generate Key Frame
       D5 - Update Frame Segment
       D6 - RSVD
       D7 - RSVD
     */

    // Video Streaming MJPEG format type descriptor
    UVC_DESC_LENGTH_VS_MJPEG_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG,       // subtype
    0x01,                                   // bFormatIndex
    0x03,                                   // bNumFrameDescriptors
    0x00,                                   // bmFlags
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming MJPEG frame type descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_848x480,          // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming MJPEG frame type descriptor: index 2
    0x32,
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x02,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x00, 0x05,                             // bWidth, 1280 = 0x0500
    0xD0, 0x02,                             // bHeight 720 = 0x02D0
    UVC_MJPEG_MIN_BITRATE_1280x720,         // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1280x720,         // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_1280x720,         // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming MJPEG frame type descriptor: index 3
    0x32,
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x03,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_1920x1080,        // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Still Image frame descriptor
    0x0b,                                       // length
    UVC_DESC_TYPE_CS_INTF,                      // type
    UVC_DESC_SUBTYPE_VS_STILL_IMAGE_FRAME,      // subtype
    0x00,                                       // bEndpointAddress
    0x01,                                       // bNumImageSizePatterns
    STILL_RES_WIDTH_6000,                       // bWidth
    STILL_RES_HEIGHT_4000,                      // bHeight
    0x01,                                       // bNumCompressionPatterns
    0x00,                                       // bCompression

    // Video Streaming Frame Based format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,  // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED, // subtype
    0x02,                                   // bFormatIndex
    0x03,                                   // bNumFrameDescriptors
    UVC_H264_GUID,
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming Frame Base Frame descriptor: index 1
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x50, 0x03,                             // bWidth, 848 = 0x0350
    0xE0, 0x01,                             // bHeight, 480 = 0x01E0
    UVC_MJPEG_MIN_BITRATE_848x480,          // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_848x480,          // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming Frame Based Frame descriptor: index 2
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x02,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x00, 0x05,                             // bWidth, 1280 = 0x0500
    0xD0, 0x02,                             // bHeight 720 = 0x02D0
    UVC_MJPEG_MIN_BITRATE_1280x720,         // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1280x720,         // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]


    // Video Streaming Frame Based Frame descriptor: index 3
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x03,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT1_ADDRESS,
    0x02,                                   // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_SS,            // max packet size
    0x00,                                   // interval

    // Super Speed Endpoint Companion descriptor
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x02,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

    // Video Streaming Interface descriptor 1
    USB_DESC_LENGTH_INTF,
    USB_DESC_TYPE_INTF,
    0x02,                                   // bInterfaceNumber
    0x00,                                   // bAlternateSetting
    0x01,                                   // bNumEndpoints
    UVC_INTF_CLASS_CODE_VIDEO,              // bInterfaceClass
    UVC_INTF_SUBCLASS_CODE_VIDEO_STREAMING, // bInterfaceSubClass
    UVC_INTF_PROTOCOL_CODE_UNDEFINE,        // bInterfaceProtocol
    0x00,                                   // iInterface

    // class-specific VS video input header descriptor
    0x0F,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x02,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT2_ADDRESS,          // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT2_ID,                             // bTerminalLink
    0x00,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
    0x00,                                   // bmaControls - H264
    /*
       D0 - Key Frame Rate
       D1 - P Frame Rate
       D2 - Compression Quality
       D3 - Compression Window Size
       D4 - Generate Key Frame
       D5 - Update Frame Segment
       D6 - RSVD
       D7 - RSVD
     */

    // Video Streaming MJPEG format type descriptor
    UVC_DESC_LENGTH_VS_MJPEG_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    0x00,                                   // bmFlags
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming MJPEG frame type descriptor: index 1
    0x32,
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_MJPEG_MAX_PAYLOAD_1920x1080,        // dwMaxVideoFrameBufSize
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bIntervalType
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // Video Streaming Frame Based format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,  // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED, // subtype
    0x02,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    UVC_H264_GUID,
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming Frame Based Frame descriptor: index 3
    0x32,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,  // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0x80, 0x07,                             // bWidth, 1920 = 0x0780
    0x38, 0x04,                             // bHeight, 1080 = 0x0438
    UVC_MJPEG_MIN_BITRATE_1920x1080,        // dwMinBitRate
    UVC_MJPEG_MAX_BITRATE_1920x1080,        // dwMaxBitRate
    UVC_FRAME_INTERVAL_30,                  // dwDefaultFrameInterval
    0x06,                                   // bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,                 // dwBytesPerLine
    UVC_FRAME_INTERVAL_30,                  // dwFrameInterval[1]
    UVC_FRAME_INTERVAL_25,                  // dwFrameInterval[2]
    UVC_FRAME_INTERVAL_20,                  // dwFrameInterval[3]
    UVC_FRAME_INTERVAL_15,                  // dwFrameInterval[4]
    UVC_FRAME_INTERVAL_10,                  // dwFrameInterval[5]
    UVC_FRAME_INTERVAL_5,                   // dwFrameInterval[6]

    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT2_ADDRESS,
    0x02,                                   // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_SS,            // max packet size
    0x00,                                   // interval

    // Super Speed Endpoint Companion descriptor
    USB_DESC_LENGTH_SS_EP_COMP,     // this descriptor size
    USB_DESC_TYPE_SS_EP_COMP,       // type
    0x02,                           // Maximum Burst
    0x00,                           // stream
    0x00, 0x00,                     // BytesPerInterval

};

static UINT8 usb_desc_strings[] __attribute__ ((aligned(32))) = {
    /* Manufacturer string descriptor : Index 1 Amba */
    0x09, 0x04,                                     // LANGID
    0x01,                                           // Index
    0x04,                                           // Length
    0x41, 0x6d, 0x62, 0x61,

    /* Product string descriptor : Index 2 Amba UVC Class */
    0x09, 0x04,                                     // LANGID
    0x02,                                           // Index
    0x20,                                           // Length
    0x41, 0x6d, 0x62, 0x61, 0x72, 0x65, 0x6c, 0x6c,
    0x61, 0x20, 0x56, 0x69, 0x64, 0x65, 0x6f, 0x20,
    0x43, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x4d, 0x4a,
    0x50, 0x45, 0x47, 0x2b, 0x48, 0x32, 0x36, 0x34, // Ambarella Video Class MJPEG+H264

    /* Serial Number string descriptor : Index 3 0001 */
    0x09, 0x04,
    0x03,
    0x04,
    0x30, 0x30, 0x30, 0x31,
};

static UINT8 usb_desc_lang_id[] __attribute__ ((aligned(32))) = {
    /* English. */
    0x09, 0x04
};


USBD_DESC_CUSTOM_INFO_s g_uvc_desc_framework = {
    .DescFrameworkFs = usb_desc_full_speed,
    .DescFrameworkHs = usb_desc_high_speed,
    .DescFrameworkSs = usb_desc_super_speed,
    .StrFramework    = usb_desc_strings,
    .LangIDFramework = usb_desc_lang_id,
    .DescSizeFs      = sizeof(usb_desc_full_speed),
    .DescSizeHs      = sizeof(usb_desc_high_speed),
    .DescSizeSs      = sizeof(usb_desc_super_speed),
    .StrSize         = sizeof(usb_desc_strings),
    .LangIDSize      = sizeof(usb_desc_lang_id),
};

