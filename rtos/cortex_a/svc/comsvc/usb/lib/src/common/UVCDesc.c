#include <StdUVC.h>

#define SPEC_VERSION_FS                   0x00, 0x02
#define SPEC_VERSION_HS                   0x00, 0x02

#define USB_VENDOR_ID                     0x55, 0x42
#define USB_PRODUCT_ID                    0x5A, 0x01
#define USB_CONFIGURATION_NUMBER          1

#define UVC_IT_ID                         0x01
#define UVC_PU_ID                         0x05
#define UVC_OT_ID                         0x03
//#define UVC_OT1_ID      0x03 // use for multiple stream
//#define UVC_OT2_ID      0x06 // use for multiple stream
//#define UVC_EU_ID       0x04
#define UVC_XU_ID                         0x0A
#define UVC_XU_LEN                        (24 + UVC_XU_NUM_PIN + UVC_XU_CONTROL_SIZE)
#define UVC_XU_CONTROL_NUM                (0xA)
#define UVC_XU_CONTROL_MAP                0xFF, 0x03 // (0x03FF)
#define UVC_XU_CONTROL_SIZE               (0x02)
#define UVC_XU_NUM_PIN                    (0x01)

//#define UVC_HEADER_POOL_NUM     50
//#define UVC_HEADER_LEN          0x0C

#define UVC_BULK_MAX_PACKET_SIZE_HS       0x00, 0x02
#define UVC_BULK_MAX_PACKET_SIZE_FS       0x40, 0x00

#define UVC_BULK_IN_ENDPOINT_ADDRESS      0x81
#define UVC_INTURRUPT_IN_ENDPOINT_ADDRESS 0x82

#define UVC_IT_CONTROL_SIZE               0x03
#define UVC_SUPPORT_IT_CONTROLS           0xAF, 0x2A, 0x06 // 0x62AAF
#define UVC_PU_CONTROL_SIZE               0x02
#define UVC_SUPPORT_PU_CONTROLS           0x7F, 0xDF       // 0xDF7F

#define UVC_YUYV_MIN_BITRATE_640x360    0x00, 0x80, 0x97, 0x06 // (0x06978000)
#define UVC_YUYV_MAX_BITRATE_640x360    0x00, 0x80, 0x97, 0x06 // (0x06978000)
#define UVC_YUYV_MAX_FRAME_BUFFER_SIZE_640x360    0x00, 0x08, 0x07, 0x00 // (0x00070800)

#define UVC_YUYV_MIN_BITRATE_1280x720    0x00, 0x00, 0x2f, 0x0D // (0x0D2F0000)
#define UVC_YUYV_MAX_BITRATE_1280x720    0x00, 0x00, 0x2f, 0x0D // (0x0D2F0000)
#define UVC_YUYV_MAX_FRAME_BUFFER_SIZE_1280x720    0x00, 0x20, 0x1C, 0x00 // (0x001C2000)

#define UVC_MJPEG_MIN_BITRATE_848x480     0x00, 0x00, 0x77, 0x01 // (0x01770000)
#define UVC_MJPEG_MIN_BITRATE_1280x720    0x00, 0x00, 0x77, 0x01 // (0x01770000)
#define UVC_MJPEG_MIN_BITRATE_1920x1080   0x00, 0x00, 0x77, 0x02 // (0x02770000)

#define UVC_MJPEG_MAX_BITRATE_848x480     0x00, 0x00, 0xCA, 0x08 // (0x08CA0000)
#define UVC_MJPEG_MAX_BITRATE_1280x720    0x00, 0x00, 0xCA, 0x08 // (0x08CA0000)
#define UVC_MJPEG_MAX_BITRATE_1920x1080   0x00, 0x00, 0xCA, 0x10 // (0x10CA0000)

#define UVC_MJPEG_MAX_PAYLOAD_848x480     0x00, 0x00, 0x00, 0x01 // (0x01000000)
#define UVC_MJPEG_MAX_PAYLOAD_1280x720    0x00, 0x60, 0x09, 0x00 // (0x00096000)
#define UVC_MJPEG_MAX_PAYLOAD_1920x1080   0x00, 0x60, 0x09, 0x00 // (0x00096000)

#define UVC_H264_MIN_BITRATE_1280x720    0x00, 0x00, 0xCA, 0x08 // (0x08CA0000)
#define UVC_H264_MAX_BITRATE_1280x720    0x00, 0x00, 0xCA, 0x08 // (0x08CA0000)

#define UVC_FRAME_INTERVAL_30             0x15, 0x16, 0x05, 0x00 // (333333 = 0x51615)
#define UVC_FRAME_INTERVAL_25             0x80, 0x1A, 0x06, 0x00 // (400000 = 0x61A80)
#define UVC_FRAME_INTERVAL_20             0x20, 0xA1, 0x07, 0x00 // (500000 = 0x7A120)
#define UVC_FRAME_INTERVAL_15             0x2A, 0x2C, 0x0A, 0x00 // (666666 = 0xA2C2A)
#define UVC_FRAME_INTERVAL_10             0x40, 0x42, 0x0F, 0x00 // (1000000 = 0xF4240)
#define UVC_FRAME_INTERVAL_5              0x80, 0x84, 0x1E, 0x00 // (2000000 = 0x1E8480)

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

#define USE_YUV
//#define USE_MJPG
//#define USE_H264
/*-----------------------------------------------------------------------------------------------*\
* Video Class 1.0 MJPEG + H264 Descriptor
\*-----------------------------------------------------------------------------------------------*/
static UINT8 appd_uvc10_descriptor_fs[] __attribute__ ((aligned(32))) =
{
    /* Device Descriptor*/
    USB_DESC_LENGTH_DEV,                  // Length
    USB_DESC_TYPE_DEV,                    // Type
    SPEC_VERSION_FS,                      // Version
    USB_DEVICE_CLASS_CODE_UVC,            // class code
    USB_DEVICE_SUBCLASS_CODE_UVC,         // subclass code
    USB_DEVICE_PROTOCOL_CODE_UVC,         // protocol code
    0x40,                                 // maximum packet size
    USB_VENDOR_ID,                        // VID
    USB_PRODUCT_ID,                       // PID
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
    0x02,                                 // bInterfaceCount
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
    UVC_DESC_LENGTH_VC_INTF,              // Length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_HEADER,           // subtype
    0x00, 0x01,                           // Spec version
    0x00, 0x00,                           // total length, calcuated later
    0x00, 0x6C, 0xDC, 0x02,               // dwClockFrequency
    0x01,                                 // bInCollection
    0x01,                                 // baInterfaceNr(1)

    // Video control output terminal descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT_ID,                            // bTerminalID
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
    0x0E,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x01,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT_ADDRESS,           // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT_ID,                              // bTerminalLink
    0x00,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
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
#if defined(USE_YUV)
    // Video Streaming YUYV format type descriptor
    UVC_DESC_LENGTH_VS_UNCOMPRESSED_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,// guidFormat
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming YUYV frame type descriptor: index 1
    0x1e,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0xAA, 0xBB, // bWidth
    0xCC, 0xDD, // bHeight
    UVC_YUYV_MIN_BITRATE_640x360,  /* dwMinBitRate         */
    UVC_YUYV_MAX_BITRATE_640x360,  /* dwMaxBitRate         */
    UVC_YUYV_MAX_FRAME_BUFFER_SIZE_640x360,  /* dwMaxVideoFrameBufSize   */
    UVC_FRAME_INTERVAL_30,   /* dwDefaultFrameInterval   */
    0x01,
    UVC_FRAME_INTERVAL_30,   /* dwFrameInterval[1]       */
#elif defined(USE_MJPG)
    // Video Streaming MJPEG format type descriptor
    UVC_DESC_LENGTH_VS_MJPEG_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    0x01,                                   // bmFlags
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming MJPEG frame type descriptor: index 1
    0x1e,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0xAA, 0xBB, // bWidth
    0xCC, 0xDD, // bHeight
    UVC_MJPEG_MIN_BITRATE_1280x720,  /* dwMinBitRate         */
    UVC_MJPEG_MIN_BITRATE_1280x720,  /* dwMaxBitRate         */
    UVC_MJPEG_MIN_BITRATE_1280x720,  /* dwMaxVideoFrameBufSize   */
    UVC_FRAME_INTERVAL_30,   /* dwDefaultFrameInterval   */
    0x01,
    UVC_FRAME_INTERVAL_30,   /* dwFrameInterval[1]       */
#else
    // Video Streaming H264 format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    UVC_H264_GUID,// guidFormat
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming H264 frame type descriptor: index 1
    0x1e,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0xAA, 0xBB, // bWidth
    0xCC, 0xDD, // bHeight
    UVC_H264_MIN_BITRATE_1280x720,  /* dwMinBitRate         */
    UVC_H264_MAX_BITRATE_1280x720,  /* dwMaxBitRate         */
    UVC_FRAME_INTERVAL_30,   /* dwDefaultFrameInterval   */
    0x01,//bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,//dwBytesPerLine
    UVC_FRAME_INTERVAL_30,   /* dwFrameInterval[1]       */
#endif
    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT_ADDRESS,
    0x02,                        // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_FS, // max packet size
    0x00,                        // interval
};

static UINT8 appd_uvc10_descriptor_hs[] __attribute__ ((aligned(32))) =
{
    /* Device Descriptor*/
    USB_DESC_LENGTH_DEV,                  // Length
    USB_DESC_TYPE_DEV,                    // Type
    SPEC_VERSION_HS,                      // Version
    USB_DEVICE_CLASS_CODE_UVC,            // class code
    USB_DEVICE_SUBCLASS_CODE_UVC,         // subclass code
    USB_DEVICE_PROTOCOL_CODE_UVC,         // protocol code
    0x40,                                 // maximum packet size
    USB_VENDOR_ID,                        // VID
    USB_PRODUCT_ID,                       // PID
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
    0x02,                                 // bInterfaceCount
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
    UVC_DESC_LENGTH_VC_INTF,              // Length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_HEADER,           // subtype
    0x00, 0x01,                           // Spec version
    0x00, 0x00,                           // total length, calcuated later
    0x00, 0x6C, 0xDC, 0x02,               // dwClockFrequency
    0x01,                                 // bInCollection
    0x01,                                 // baInterfaceNr(1)

    // Video control output terminal descriptor
    UVC_DESC_LENGTH_OUTPUT_TERMINAL,      // length
    UVC_DESC_TYPE_CS_INTF,                // type
    UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL,  // subtype
    UVC_OT_ID,                            // bTerminalID
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
    0x0e,                                    //(UVC_DESC_LENGTH_VS_INPUT_HEADER + 1U),
    UVC_DESC_TYPE_CS_INTF,
    UVC_DESC_SUBTYPE_VS_INPUT_HEADER,       // bDescriptorSubtype
    0x01,                                   // bNumFormats
    0x4B, 0x00,                             // wTotalLength
    UVC_BULK_IN_ENDPOINT_ADDRESS,           // bEndpointAddress
    0x00,                                   // bmInfo
    UVC_OT_ID,                              // bTerminalLink
    0x00,                                   // bStillCaptureMethod
    0x00,                                   // bTriggerSupport
    0x00,                                   // bTriggerUsage
    0x01,                                   // bControlSize
    0x04,                                   // bmaControls - MJPEG
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
#if defined(USE_YUV)
    // Video Streaming YUYV format type descriptor
    UVC_DESC_LENGTH_VS_UNCOMPRESSED_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71,// guidFormat
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect

    // Video Streaming YUYV frame type descriptor: index 1
    0x1e,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0xAA, 0xBB, // bWidth
    0xCC, 0xDD, // bHeight
    UVC_YUYV_MIN_BITRATE_640x360,  /* dwMinBitRate         */
    UVC_YUYV_MAX_BITRATE_640x360,  /* dwMaxBitRate         */
    UVC_YUYV_MAX_FRAME_BUFFER_SIZE_640x360,  /* dwMaxVideoFrameBufSize   */
    UVC_FRAME_INTERVAL_30,   /* dwDefaultFrameInterval   */
    0x01,
    UVC_FRAME_INTERVAL_30,   /* dwFrameInterval[1]       */
#elif defined(USE_MJPG)
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
    0x1e,
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_MJPEG,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0xAA, 0xBB, // bWidth
    0xCC, 0xDD, // bHeight
    UVC_MJPEG_MIN_BITRATE_1280x720,   /* dwMinBitRate         */
    UVC_MJPEG_MIN_BITRATE_1280x720,   /* dwMaxBitRate         */
    UVC_MJPEG_MIN_BITRATE_1280x720,   /* dwMaxVideoFrameBufSize   */
    UVC_FRAME_INTERVAL_30,   /* dwDefaultFrameInterval   */
    0x01,
    UVC_FRAME_INTERVAL_30,   /* dwFrameInterval[1]       */
#else
    // Video Streaming H264 format type descriptor
    UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT,        // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED,       // subtype
    0x01,                                   // bFormatIndex
    0x01,                                   // bNumFrameDescriptors
    UVC_H264_GUID,// guidFormat
    0x10,                                   // bBitsPerPixel
    0x01,                                   // bDefaultFrameIndex
    0x00,                                   // bAspectRatioX
    0x00,                                   // bAspectRatioY
    0x00,                                   // bmInterlaceFlags
    0x00,                                   // bCopyProtect
    0x01,                                   // bVariableSize

    // Video Streaming H264 frame type descriptor: index 1
    0x1e,                                   // length
    UVC_DESC_TYPE_CS_INTF,                  // type
    UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED,        // subtype
    0x01,                                   // bFrameIndex
    0x00,                                   // bmCapabilities
    0xAA, 0xBB, // bWidth
    0xCC, 0xDD, // bHeight
    UVC_H264_MIN_BITRATE_1280x720,  /* dwMinBitRate         */
    UVC_H264_MAX_BITRATE_1280x720,  /* dwMaxBitRate         */
    UVC_FRAME_INTERVAL_30,   /* dwDefaultFrameInterval   */
    0x01,//bFrameIntervalType
    0x00, 0x00, 0x00, 0x00,//dwBytesPerLine
    UVC_FRAME_INTERVAL_30,   /* dwFrameInterval[1]       */
#endif
    // BULK Endpoint descriptor
    USB_DESC_LENGTH_ENDPOINT,
    USB_DESC_TYPE_ENDPOINT,
    UVC_BULK_IN_ENDPOINT_ADDRESS,
    0x02,                        // bmAttribute
    UVC_BULK_MAX_PACKET_SIZE_HS, // max packet size
    0x00,                        // interval
};

static UINT8 uvc10_strings[] __attribute__ ((aligned(32))) =
{
    /* Manufacturer string descriptor : Index 1 Amba */
    0x09, 0x04,                                     // LANGID
    0x01,                                           // Index
    0x04,                                           // Length
    0x41, 0x6d, 0x62, 0x61,

    /* Product string descriptor : Index 2 Amba UVC Class */
    0x09, 0x04,                                     // LANGID
    0x02,                                           // Index
    0x10,                                           // Length
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

static UINT8 Languages[] __attribute__ ((aligned(32))) =
{
    /* English. */
    0x09, 0x04
};

USBD_DESC_CUSTOM_INFO_s UvcDescFramework =
{
    .DescFrameworkFs = appd_uvc10_descriptor_fs,
    .DescFrameworkHs = appd_uvc10_descriptor_hs,
    .StrFramework    = uvc10_strings,
    .LangIDFramework = Languages,
    .DescSizeFs      = sizeof(appd_uvc10_descriptor_fs),
    .DescSizeHs      = sizeof(appd_uvc10_descriptor_hs),
    .StrSize         = sizeof(uvc10_strings),
    .LangIDSize      = sizeof(Languages),
};

