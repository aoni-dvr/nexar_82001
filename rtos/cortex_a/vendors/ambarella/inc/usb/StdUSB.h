/**
 *  @file StdUSB.h
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
 *  @details Standard USB header file.
 */

#ifndef STD_USB_H
#define STD_USB_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

//---------------------------------------------
// Test Mode Selectors
// See USB 2.0 spec Table 9-7
//---------------------------------------------

#define TEST_J                          1U
#define TEST_K                          2U
#define TEST_SE0_NAK                    3U
#define TEST_PACKET                     4U
#define TEST_FORCE_EN                   5U

#define USB_ENDPOINT_NUMBER_MASK        0x0FU    // in bEndpointAddress
#define USB_ENDPOINT_DIR_MASK           0x80U

#define USB_ENDPOINT_XFERTYPE_MASK      0x03U    // in bmAttributes
#define USB_ENDPOINT_XFER_CONTROL       0U
#define USB_ENDPOINT_XFER_ISOC          1U
#define USB_ENDPOINT_XFER_BULK          2U
#define USB_ENDPOINT_XFER_INT           3U
#define USB_ENDPOINT_MAX_ADJUSTABLE     0x80U

#define USB_TYPE_MASK                   (0x03U << 5U)
#define USB_TYPE_STANDARD               (0x00U << 5U)
#define USB_TYPE_CLASS                  (0x01U << 5U)
#define USB_TYPE_VENDOR                 (0x02U << 5U)
#define USB_TYPE_RESERVED               (0x03U << 5U)

#define USB_RECIP_MASK                  0x1fU
#define USB_RECIP_DEVICE                0x00U
#define USB_RECIP_INTERFACE             0x01U
#define USB_RECIP_ENDPOINT              0x02U
#define USB_RECIP_OTHER

// Standard requests, for the bRequest field of a SETUP packet.
//
// These are qualified by the bRequestType field, so that for example
// TYPE_CLASS or TYPE_VENDOR specific feature flags could be retrieved
// by a GET_STATUS request.

#define USB_REQ_GET_STATUS              0x00U
#define USB_REQ_CLEAR_FEATURE           0x01U
#define USB_REQ_SET_FEATURE             0x03U
#define USB_REQ_SET_ADDRESS             0x05U
#define USB_REQ_GET_DESCRIPTOR          0x06U
#define USB_REQ_SET_DESCRIPTOR          0x07U
#define USB_REQ_GET_CONFIGURATION       0x08U
#define USB_REQ_SET_CONFIGURATION       0x09U
#define USB_REQ_GET_INTERFACE           0x0AU
#define USB_REQ_SET_INTERFACE           0x0BU
#define USB_REQ_SYNCH_FRAME             0x0CU
#define USB_REQ_SET_SEL                 0x30U
#define USB_REQ_SET_ISOCH_DELAY         0x31U

// USB setup request structure
typedef struct {
    UINT8 bmRequestType;
    UINT8 bRequest;
    UINT16 wValue;
    UINT16 wIndex;
    UINT16 wLength;
} USB_SETUP_REQUEST_S;

// USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE, and
// are read as a bit array returned by USB_REQ_GET_STATUS.  (So there
// are at most sixteen features of each type.)  Hubs may also support a
// new USB_REQ_TEST_AND_SET_FEATURE to put ports into L1 suspend.

#define USB_DEVICE_SELF_POWERED         0U       // (read only)
#define USB_DEVICE_REMOTE_WAKEUP        1U       // dev may initiate wakeup
#define USB_DEVICE_TEST_MODE            2U       // (wired high speed only)
#define USB_DEVICE_BATTERY              2U       // (wireless)
#define USB_DEVICE_B_HNP_ENABLE         3U       // (otg) dev may initiate HNP
#define USB_DEVICE_WUSB_DEVICE          3U       // (wireless)
#define USB_DEVICE_A_HNP_SUPPORT        4U       // (otg) RH port supports HNP
#define USB_DEVICE_A_ALT_HNP_SUPPORT    5U       // (otg) other RH port does
#define USB_DEVICE_DEBUG_MODE           6U       // (special devices only)
#define USB_ENDPOINT_HALT               0U       // IN/OUT will STALL */

#define DESC_EP_ATTR_CONTROL            0U
#define DESC_EP_ATTR_ISO                0x01U
#define DESC_EP_ATTR_BULK               0x02U
#define DESC_EP_ATTR_INT                0x03U
#define DESC_EP_DIR_OUT                 0x00U
#define DESC_EP_DIR_IN                  0x80U

#define USB_DESC_LENGTH_DEV             0x12U
#define USB_DESC_LENGTH_BOS             0x05U
#define USB_DESC_LENGTH_CFG             0x09U
#define USB_DESC_LENGTH_INTF            0x09U
#define USB_DESC_LENGTH_QUALIFIER       0x0AU
#define USB_DESC_LENGTH_ENDPOINT        0x07U
#define USB_DESC_LENGTH_SS_EP_COMP      0x06U
#define USB_DESC_LENGTH_IAD             0x08U
#define USB_DESC_LENGTH_ISO_EP_COMP     0x08U

// Descriptor length of device capability type
#define USB_DESC_LENGTH_CAP_USB20EX     0x07U
#define USB_DESC_LENGTH_CAP_SS_USB      0x0AU
#define USB_DESC_LENGTH_CAP_SSP_USB     0x14U

#define BOS_DESCRIPTOR_LENGTH_SS        (USB_DESC_LENGTH_BOS + \
                                        USB_DESC_LENGTH_CAP_USB20EX + \
                                        USB_DESC_LENGTH_CAP_SS_USB)

#define BOS_DESCRIPTOR_LENGTH_SSP       (USB_DESC_LENGTH_BOS + \
                                        USB_DESC_LENGTH_CAP_USB20EX + \
                                        USB_DESC_LENGTH_CAP_SS_USB + \
                                        USB_DESC_LENGTH_CAP_SSP_USB)

#define USB_DESC_TYPE_DEV               0x01U
#define USB_DESC_TYPE_CFG               0x02U
#define USB_DESC_TYPE_STRING            0x03U
#define USB_DESC_TYPE_INTF              0x04U
#define USB_DESC_TYPE_ENDPOINT          0x05U
#define USB_DESC_TYPE_QUALIFIER         0x06U
#define USB_DESC_TYPE_IAD               0x0BU
#define USB_DESC_TYPE_CS_INTERFACE      0x24U
#define USB_DESC_TYPE_CS_ENDPOINT       0x25U
#define USB_DESC_TYPE_BOS               0x0FU
#define USB_DESC_TYPE_DEV_CAP           0x10U
#define USB_DESC_TYPE_SS_EP_COMP        0x30U
#define USB_DESC_TYPE_SSP_ISO_COMP      0x31U

#define USB_DESC_CAP_TYPE_USB20EX       0x02U
#define USB_DESC_CAP_TYPE_SS_USB        0x03U
#define USB_DESC_CAP_TYPE_SSP_USB       0x0AU
#define USB_DESC_CAP_NUM                0x02U       // Default is USB20_EX and SS_USB.

#define USB_DEVICE_CLASS_MISC           0xEFU
#define USB_DEVICE_SUBCLASS_IAD         0x02U
#define USB_DEVICE_PROTOCOL_IAD         0x01U

#define USB_DEVICE_CLASS_NONE           0x00U
#define USB_DEVICE_SUBCLASS_NONE        0x00U
#define USB_DEVICE_PROTOCOL_NONE        0x00U

// ----------------------------------------
//   CDC device class
// ----------------------------------------
#define USB_DEVICE_CLASS_CDC            0x02U
#define USB_DEVICE_SUBCLASS_CDC         0x00U
#define USB_DEVICE_PROTOCOL_CDC         0x00U

// CDC Subclass Codes
#define USB_INTERFACE_CLASS_CDC         0x02U
#define USB_INTERFACE_CLASS_CDC_DATA    0x0AU

#define USB_CDC_SUBCLASS_DLCM           0x01U
#define USB_CDC_SUBCLASS_ACM            0x02U

// CDC  Protocol Codes
#define USB_CDC_PROTOCOL_V250               0x01U
#define USB_CDC_PROTOCOL_PCCA_101           0x02U
#define USB_CDC_PROTOCOL_PCCA_101_ANNEXO    0x03U
#define USB_CDC_PROTOCOL_GSM707             0x04U
#define USB_CDC_PROTOCOL_3GPP               0x05U
#define USB_CDC_PROTOCOL_CS00170            0x06U
#define USB_CDC_PROTOCOL_EEM                0x07U

// CDC Descriptor Subtype
#define USB_CDC_DESC_SUBTYPE_HEADER_FUNC     0x00U
#define USB_CDC_DESC_SUBTYPE_CALL_MANAGEMENT 0x01U
#define USB_CDC_DESC_SUBTYPE_ACM_FUNC        0x02U
#define USB_CDC_DESC_SUBTYPE_UNION_FUNC      0x06U

// ----------------------------------------
//   Still Inage device class
// ----------------------------------------
#define USB_DEVICE_CLASS_STILL_IMAGE          0x06U
#define USB_DEVICE_SUBCLASS_STILL_IMAGE       0x01U
#define USB_DEVICE_PROTOCOL_STILL_IMAGE       0x01U

// ----------------------------------------
//   MSC device class
// ----------------------------------------
#define USB_DEVICE_CLASS_MSC          0x08U
#define USB_DEVICE_SUBCLASS_MSC_SCSI  0x06U
#define USB_DEVICE_PROTOCOL_MSC_BOT   0x50U // Bulk-Only Transport

/* Define MTP Storage Types Codes.  */

#define MTP_STC_UNDEFINED                                     0x0000U
#define MTP_STC_FIXED_ROM                                     0x0001U
#define MTP_STC_REMOVABLE_ROM                                 0x0002U
#define MTP_STC_FIXED_RAM                                     0x0003U
#define MTP_STC_REMOVABLE_RAM                                 0x0004U

/* Define MTP File System Types Codes.  */

#define MTP_FSTC_UNDEFINED                                    0x0000U
#define MTP_FSTC_GENERIC_FLAT                                 0x0001U
#define MTP_FSTC_GENERIC_HIERARCHICAL                         0x0002U
#define MTP_FSTC_DCF                                          0x0003U

/* Define MTP File System Access Types Codes.  */

#define MTP_AC_READ_WRITE                                     0x0000U
#define MTP_AC_RO_WITHOUT_OBJECT_DELETION                     0x0001U
#define MTP_AC_RO_WITH_OBJECT_DELETION                        0x0002U

// Define MTP Association Sub-Type
#define MTP_ASSOCIATION_TYPE_UNDEFINED                            0x0000U
#define MTP_ASSOCIATION_TYPE_GENERIC_FOLDER                     0x0001U
#define MTP_ASSOCIATION_TYPE_ALBUM                              0x0002U
#define MTP_ASSOCIATION_TYPE_TIME_SEQUENCE                      0x0003U
#define MTP_ASSOCIATION_TYPE_HORIZONTAL_PANARAMIC               0x0004U
#define MTP_ASSOCIATION_TYPE_VERTICAL_PANARAMIC                 0x0005U
#define MTP_ASSOCIATION_TYPE_2D_PANARAMIC                       0x0006U
#define MTP_ASSOCIATION_TYPE_ANCILLARY_DATA                     0x0007U

// Define MTP Object Format Codes.
#define MTP_OFC_UNDEFINED                                       0x3000U
#define MTP_OFC_ASSOCIATION                                     0x3001U
#define MTP_OFC_SCRIPT                                          0x3002U
#define MTP_OFC_EXECUTABLE                                      0x3003U
#define MTP_OFC_TEXT                                            0x3004U
#define MTP_OFC_HTML                                            0x3005U
#define MTP_OFC_DPOF                                            0x3006U
#define MTP_OFC_AIFF                                            0x3007U
#define MTP_OFC_WAV                                             0x3008U
#define MTP_OFC_MP3                                             0x3009U
#define MTP_OFC_AVI                                             0x300AU
#define MTP_OFC_MPEG                                            0x300BU
#define MTP_OFC_ASF                                             0x300CU
#define MTP_OFC_DEFINED                                         0x3800U
#define MTP_OFC_EXIF_JPEG                                       0x3801U
#define MTP_OFC_TIFF_EP                                         0x3802U
#define MTP_OFC_FLASHPIX                                        0x3803U
#define MTP_OFC_BMP                                             0x3804U
#define MTP_OFC_CIFF                                            0x3805U
#define MTP_OFC_UNDEFINED_2                                     0x3806U
#define MTP_OFC_GIF                                             0x3807U
#define MTP_OFC_JFIF                                            0x3808U
#define MTP_OFC_CD                                              0x3809U
#define MTP_OFC_PICT                                            0x380AU
#define MTP_OFC_PNG                                             0x380BU
#define MTP_OFC_UNDEFINED_3                                     0x380CU
#define MTP_OFC_TIFF                                            0x380DU
#define MTP_OFC_TIFF_IT                                         0x380EU
#define MTP_OFC_JP2                                             0x380FU
#define MTP_OFC_JPX                                             0x3810U
#define MTP_OFC_UNDEFINED_FIRMWARE                              0xB802U
#define MTP_OFC_WINDOWS_IMAGE_FORMAT                            0xB881U
#define MTP_OFC_UNDEFINED_AUDIO                                 0xB900U
#define MTP_OFC_WMA                                             0xB901U
#define MTP_OFC_OGG                                             0xB902U
#define MTP_OFC_AAC                                             0xB903U
#define MTP_OFC_AUDIBLE                                         0xB904U
#define MTP_OFC_FLAC                                            0xB906U
#define MTP_OFC_UNDEFINED_VIDEO                                 0xB980U
#define MTP_OFC_WMV                                             0xB981U
#define MTP_OFC_MP4_CONTAINER                                   0xB982U
#define MTP_OFC_MP2                                             0xB983U
#define MTP_OFC_3GP_CONTAINER                                   0xB984U
#define MTP_OFC_UNDEFINED_COLLECTION                            0xBA00U
#define MTP_OFC_ABSTRACT_MULTIMEDIA_ALBUM                       0xBA01U
#define MTP_OFC_ABSTRACT_IMAGE_ALBUM                            0xBA02U
#define MTP_OFC_ABSTRACT_AUDIO_ALBUM                            0xBA03U
#define MTP_OFC_ABSTRACT_VIDEO_ALBUM                            0xBA04U
#define MTP_OFC_ABSTRACT_AUDIO_AND_VIDEO_PLAYLIST               0xBA05U
#define MTP_OFC_ABSTRACT_CONTACT_GROUP                          0xBA06U
#define MTP_OFC_ABSTRACT_MESSAGE_FOLDER                         0xBA07U
#define MTP_OFC_ABSTRACT_CHAPTERED_PRODUCTION                   0xBA08U
#define MTP_OFC_ABSTRACT_AUDIO_PLAYLIST                         0xBA09U
#define MTP_OFC_ABSTRACT_VIDEO_PLAYLIST                         0xBA0AU
#define MTP_OFC_ABSTRACT_MEDIACAST                              0xBA0BU
#define MTP_OFC_WPL_PLAYLIST                                    0xBA10U
#define MTP_OFC_M3U_PLAYLIST                                    0xBA11U
#define MTP_OFC_MPL_PLAYLIST                                    0xBA12U
#define MTP_OFC_ASX_PLAYLIST                                    0xBA13U
#define MTP_OFC_PLS_PLAYLIST                                    0xBA14U
#define MTP_OFC_UNDEFINED_DOCUMENT                              0xBA80U
#define MTP_OFC_ABSTRACT_DOCUMENT                               0xBA81U
#define MTP_OFC_XML_DOCUMENT                                    0xBA82U
#define MTP_OFC_MICROSOFT_WORD_DOCUMENT                         0xBA83U
#define MTP_OFC_MHT_COMPILED_HTML_DOCUMENT                      0xBA84U
#define MTP_OFC_MICROSOFT_EXCEL_SPREADSHEET                     0xBA85U
#define MTP_OFC_MICROSOFT_POWERPOINT_PRESENTATION               0xBA86U
#define MTP_OFC_UNDEFINED_MESSAGE                               0xBB00U
#define MTP_OFC_ABSTRACT_MESSAGE                                0xBB01U
#define MTP_OFC_UNDEFINED_CONTACT                               0xBB80U
#define MTP_OFC_ABSTRACT_CONTACT                                0xBB81U
#define MTP_OFC_VCARD2                                          0xBB82U

// Define MTP Device Property Codes.

#define MTP_DEV_PROP_UNDEFINED                                  0x5000U
#define MTP_DEV_PROP_BATTERY_LEVEL                              0x5001U
#define MTP_DEV_PROP_FUNCTIONAL_MODE                            0x5002U
#define MTP_DEV_PROP_IMAGE_SIZE                                 0x5003U
#define MTP_DEV_PROP_COMPRESSION_SETTING                        0x5004U
#define MTP_DEV_PROP_WHITE_BALANCE                              0x5005U
#define MTP_DEV_PROP_RGB_GAIN                                   0x5006U
#define MTP_DEV_PROP_F_NUMBER                                   0x5007U
#define MTP_DEV_PROP_FOCAL_LENGTH                               0x5008U
#define MTP_DEV_PROP_FOCUS_DISTANCE                             0x5009U
#define MTP_DEV_PROP_FOCUS_MODE                                 0x500AU
#define MTP_DEV_PROP_EXPOSURE_METERING_MODE                     0x500BU
#define MTP_DEV_PROP_FLASH_MODE                                 0x500CU
#define MTP_DEV_PROP_EXPOSURE_TIME                              0x500DU
#define MTP_DEV_PROP_EXPOSURE_PROGRAM_MODE                      0x500EU
#define MTP_DEV_PROP_EXPOSURE_INDEX                             0x500FU
#define MTP_DEV_PROP_EXPOSURE_BIAS_COMPENSATION                 0x5010U
#define MTP_DEV_PROP_DATE_TIME                                  0x5011U
#define MTP_DEV_PROP_CAPTURE_DELAY                              0x5012U
#define MTP_DEV_PROP_STILL_CAPTURE_MODE                         0x5013U
#define MTP_DEV_PROP_CONTRAST                                   0x5014U
#define MTP_DEV_PROP_SHARPNESS                                  0x5015U
#define MTP_DEV_PROP_DIGITAL_ZOOM                               0x5016U
#define MTP_DEV_PROP_EFFECT_MODE                                0x5017U
#define MTP_DEV_PROP_BURST_NUMBER                               0x5018U
#define MTP_DEV_PROP_BURST_INTERVAL                             0x5019U
#define MTP_DEV_PROP_TIME_LAPSE_NUMBER                          0x501AU
#define MTP_DEV_PROP_TIME_LAPSE_INTERVAL                        0x501BU
#define MTP_DEV_PROP_FOCUS_METERING_MODE                        0x501CU
#define MTP_DEV_PROP_UPLOAD_URL                                 0x501DU
#define MTP_DEV_PROP_ARTIST                                     0x501EU
#define MTP_DEV_PROP_COPYRIGHT_INFO                             0x501FU
#define MTP_DEV_PROP_USE_DEVICE_STAGE_FLAG                      0xD303U
#define MTP_DEV_PROP_SYNCHRONIZATION_PARTNER                    0xD401U
#define MTP_DEV_PROP_DEVICE_FRIENDLY_NAME                       0xD402U
#define MTP_DEV_PROP_VOLUME                                     0xD403U
#define MTP_DEV_PROP_SUPPORTED_FORMATS_ORDERED                  0xD404U
#define MTP_DEV_PROP_DEVICE_ICON                                0xD405U
#define MTP_DEV_PROP_PLAYBACK_RATE                              0xD410U
#define MTP_DEV_PROP_PLAYBACK_OBJECT                            0xD411U
#define MTP_DEV_PROP_PLAYBACK_CONTAINER                         0xD412U
#define MTP_DEV_PROP_SESSION_INITIATOR_VERSION_INFO             0xD406U
#define MTP_DEV_PROP_PERCEIVED_DEVICE_TYPE                      0xD407U


// Define MTP Object Property Codes.

#define MTP_OBJECT_PROP_STORAGEID                               0xDC01U
#define MTP_OBJECT_PROP_OBJECT_FORMAT                           0xDC02U
#define MTP_OBJECT_PROP_PROTECTION_STATUS                       0xDC03U
#define MTP_OBJECT_PROP_OBJECT_SIZE                             0xDC04U
#define MTP_OBJECT_PROP_ASSOCIATION_TYPE                        0xDC05U
#define MTP_OBJECT_PROP_ASSOCIATION_DESC                        0xDC06U
#define MTP_OBJECT_PROP_OBJECT_FILE_NAME                        0xDC07U
#define MTP_OBJECT_PROP_DATE_CREATED                            0xDC08U
#define MTP_OBJECT_PROP_DATE_MODIFIED                           0xDC09U
#define MTP_OBJECT_PROP_KEYWORDS                                0xDC0AU
#define MTP_OBJECT_PROP_PARENT_OBJECT                           0xDC0BU
#define MTP_OBJECT_PROP_ALLOWED_FOLDER_CONTENTS                 0xDC0CU
#define MTP_OBJECT_PROP_HIDDEN                                  0xDC0DU
#define MTP_OBJECT_PROP_SYSTEM_OBJECT                           0xDC0EU
#define MTP_OBJECT_PROP_PERSISTENT_UNIQUE_OBJECT_IDENTIFIER     0xDC41U
#define MTP_OBJECT_PROP_SYNCID                                  0xDC42U
#define MTP_OBJECT_PROP_PROPERTY_BAG                            0xDC43U
#define MTP_OBJECT_PROP_NAME                                    0xDC44U
#define MTP_OBJECT_PROP_CREATED_BY                              0xDC45U
#define MTP_OBJECT_PROP_ARTIST                                  0xDC46U
#define MTP_OBJECT_PROP_DATE_AUTHORED                           0xDC47U
#define MTP_OBJECT_PROP_DESCRIPTION                             0xDC48U
#define MTP_OBJECT_PROP_URL_REFERENCE                           0xDC49U
#define MTP_OBJECT_PROP_LANGUAGE_LOCALE                         0xDC4AU
#define MTP_OBJECT_PROP_COPYRIGHT_INFORMATION                   0xDC4BU
#define MTP_OBJECT_PROP_SOURCE                                  0xDC4CU
#define MTP_OBJECT_PROP_ORIGIN_LOCATION                         0xDC4DU
#define MTP_OBJECT_PROP_DATE_ADDED                              0xDC4EU
#define MTP_OBJECT_PROP_NON_CONSUMABLE                          0xDC4FU
#define MTP_OBJECT_PROP_CORRUPT_UNPLAYABLE                      0xDC50U
#define MTP_OBJECT_PROP_PRODUCER_SERIA_LNUMBER                  0xDC51U
#define MTP_OBJECT_PROP_REPRE_SMPL_FORMAT                       0xDC81U // REPRE_SMPL -> REPRESENTATIVE_SAMPLE
#define MTP_OBJECT_PROP_REPRE_SMPL_SIZE                         0xDC82U
#define MTP_OBJECT_PROP_REPRE_SMPL_HEIGHT                       0xDC83U
#define MTP_OBJECT_PROP_REPRE_SMPL_WIDTH                        0xDC84U
#define MTP_OBJECT_PROP_REPRE_SMPL_DURATION                     0xDC85U
#define MTP_OBJECT_PROP_REPRE_SMPL_DATA                         0xDC86U
#define MTP_OBJECT_PROP_WIDTH                                   0xDC87U
#define MTP_OBJECT_PROP_HEIGHT                                  0xDC88U
#define MTP_OBJECT_PROP_DURATION                                0xDC89U
#define MTP_OBJECT_PROP_RATING                                  0xDC8AU
#define MTP_OBJECT_PROP_TRACK                                   0xDC8BU
#define MTP_OBJECT_PROP_GENRE                                   0xDC8CU
#define MTP_OBJECT_PROP_CREDITS                                 0xDC8DU
#define MTP_OBJECT_PROP_LYRICS                                  0xDC8EU
#define MTP_OBJECT_PROP_SUBSCRIPTION_CONTENT_ID                 0xDC8FU
#define MTP_OBJECT_PROP_PRODUCED_BY                             0xDC90U
#define MTP_OBJECT_PROP_USE_COUNT                               0xDC91U
#define MTP_OBJECT_PROP_SKIP_COUNT                              0xDC92U
#define MTP_OBJECT_PROP_LAST_ACCESSED                           0xDC93U
#define MTP_OBJECT_PROP_PARENTAL_RATING                         0xDC94U
#define MTP_OBJECT_PROP_META_GENRE                              0xDC95U
#define MTP_OBJECT_PROP_COMPOSER                                0xDC96U
#define MTP_OBJECT_PROP_EFFECTIVE_RATING                        0xDC97U
#define MTP_OBJECT_PROP_SUBTITLE                                0xDC98U
#define MTP_OBJECT_PROP_ORIGINAL_RELEASE_DATE                   0xDC99U
#define MTP_OBJECT_PROP_ALBUM_NAME                              0xDC9AU
#define MTP_OBJECT_PROP_ALBUM_ARTIST                            0xDC9BU
#define MTP_OBJECT_PROP_MOOD                                    0xDC9CU
#define MTP_OBJECT_PROP_DRM_STATUS                              0xDC9DU
#define MTP_OBJECT_PROP_SUB_DESCRIPTION                         0xDC9EU
#define MTP_OBJECT_PROP_IS_CROPPED                              0xDCD1U
#define MTP_OBJECT_PROP_IS_COLOUR_CORRECTED                     0xDCD2U
#define MTP_OBJECT_PROP_IMAGE_BIT_DEPTH                         0xDCD3U
#define MTP_OBJECT_PROP_FNUMBER                                 0xDCD4U
#define MTP_OBJECT_PROP_EXPOSURE_TIME                           0xDCD5U
#define MTP_OBJECT_PROP_EXPOSURE_INDEX                          0xDCD6U
#define MTP_OBJECT_PROP_TOTAL_BITRATE                           0xDE91U
#define MTP_OBJECT_PROP_BITRATE_TYPE                            0xDE92U
#define MTP_OBJECT_PROP_SAMPLE_RATE                             0xDE93U
#define MTP_OBJECT_PROP_NUMBER_OF_CHANNELS                      0xDE94U
#define MTP_OBJECT_PROP_AUDIO_BITDEPTH                          0xDE95U
#define MTP_OBJECT_PROP_SCAN_TYPE                               0xDE97U
#define MTP_OBJECT_PROP_AUDIO_WAVE_CODEC                        0xDE99U
#define MTP_OBJECT_PROP_AUDIO_BITRATE                           0xDE9AU
#define MTP_OBJECT_PROP_VIDEO_FOURCC_CODEC                      0xDE9BU
#define MTP_OBJECT_PROP_VIDEO_BITRATE                           0xDE9CU
#define MTP_OBJECT_PROP_FRAMES_PER_THOUSAND_SECONDS             0xDE9DU
#define MTP_OBJECT_PROP_KEYFRAME_DISTANCE                       0xDE9EU
#define MTP_OBJECT_PROP_BUFFER_SIZE                             0xDE9FU
#define MTP_OBJECT_PROP_ENCODING_QUALITY                        0xDEA0U
#define MTP_OBJECT_PROP_ENCODING_PROFILE                        0xDEA1U
#define MTP_OBJECT_PROP_DISPLAY_NAME                            0xDCE0U
#define MTP_OBJECT_PROP_BODY_TEXT                               0xDCE1U
#define MTP_OBJECT_PROP_SUBJECT                                 0xDCE2U
#define MTP_OBJECT_PROP_PRIORITY                                0xDCE3U
#define MTP_OBJECT_PROP_GIVEN_NAME                              0xDD00U
#define MTP_OBJECT_PROP_MIDDLE_NAMES                            0xDD01U
#define MTP_OBJECT_PROP_FAMILY_NAME                             0xDD02U
#define MTP_OBJECT_PROP_PREFIX                                  0xDD03U
#define MTP_OBJECT_PROP_SUFFIX                                  0xDD04U
#define MTP_OBJECT_PROP_PHONETIC_GIVEN_NAME                     0xDD05U
#define MTP_OBJECT_PROP_PHONETIC_FAMILY_NAME                    0xDD06U
#define MTP_OBJECT_PROP_EMAIL_PRIMARY                           0xDD07U
#define MTP_OBJECT_PROP_EMAIL_PER1                              0xDD08U // EMAIL_PER -> EMAIL_PERSONAL
#define MTP_OBJECT_PROP_EMAIL_PER2                              0xDD09U
#define MTP_OBJECT_PROP_EMAIL_BUS1                              0xDD0AU // EMAIL_BUS -> EMAIL_BUSINESS
#define MTP_OBJECT_PROP_EMAIL_BUS2                              0xDD0BU
#define MTP_OBJECT_PROP_EMAIL_OTHERS                            0xDD0CU
#define MTP_OBJECT_PROP_PNUM_PRI                                0xDD0DU // PNUM_PRI -> PHONE_NUMBER_PRIMARY
#define MTP_OBJECT_PROP_PNUM_PER                                0xDD0EU // PNUM_PER -> PHONE_NUMBER_PERSONAL
#define MTP_OBJECT_PROP_PNUM_PER2                               0xDD0FU
#define MTP_OBJECT_PROP_PNUM_BUS                                0xDD10U // PNUM_BUS -> PHONE_NUMBER_BUSINESS
#define MTP_OBJECT_PROP_PNUM_BUS2                               0xDD11U
#define MTP_OBJECT_PROP_PNUM_MOB                                0xDD12U // PNUM_MOB -> PHONE_NUMBER_MOBILE
#define MTP_OBJECT_PROP_PNUM_MOB2                               0xDD13U
#define MTP_OBJECT_PROP_FAX_NUMBER_PRIMARY                      0xDD14U
#define MTP_OBJECT_PROP_FAX_NUMBER_PERSONAL                     0xDD15U
#define MTP_OBJECT_PROP_FAX_NUMBER_BUSINESS                     0xDD16U
#define MTP_OBJECT_PROP_PAGER_NUMBER                            0xDD17U
#define MTP_OBJECT_PROP_PHONE_NUMBER_OTHERS                     0xDD18U
#define MTP_OBJECT_PROP_PRIMARY_WEB_ADDRESS                     0xDD19U
#define MTP_OBJECT_PROP_PERSONAL_WEB_ADDRESS                    0xDD1AU
#define MTP_OBJECT_PROP_BUSINESS_WEB_ADDRESS                    0xDD1BU
#define MTP_OBJECT_PROP_INST_MSN_ADDR                           0xDD1CU // INST_MSN_ADDR -> INSTANT_MESSENGER_ADDRESS
#define MTP_OBJECT_PROP_INST_MSN_ADDR2                          0xDD1DU
#define MTP_OBJECT_PROP_INST_MSN_ADDR3                          0xDD1EU
#define MTP_OBJECT_PROP_PADDR_PER_FULL                          0xDD1FU // PADDR -> POSTAL ADDRESS
#define MTP_OBJECT_PROP_PADDR_PER_LINE1                         0xDD20U // PER -> PERSONAL
#define MTP_OBJECT_PROP_PADDR_PER_LINE2                         0xDD21U
#define MTP_OBJECT_PROP_PADDR_PER_CITY                          0xDD22U
#define MTP_OBJECT_PROP_PADDR_PER_REGION                        0xDD23U
#define MTP_OBJECT_PROP_PADDR_PER_POSTAL_CODE                   0xDD24U
#define MTP_OBJECT_PROP_PADDR_PER_COUNTRY                       0xDD25U
#define MTP_OBJECT_PROP_PADDR_BUS_FULL                          0xDD26U // BUS -> BUSINESS
#define MTP_OBJECT_PROP_PADDR_BUS_LINE1                         0xDD27U
#define MTP_OBJECT_PROP_PADDR_BUS_LINE2                         0xDD28U
#define MTP_OBJECT_PROP_PADDR_BUS_CITY                          0xDD29U
#define MTP_OBJECT_PROP_PADDR_BUS_REGION                        0xDD2AU
#define MTP_OBJECT_PROP_PADDR_BUS_POSTAL_CODE                   0xDD2BU
#define MTP_OBJECT_PROP_PADDR_BUS_COUNTRY                       0xDD2CU
#define MTP_OBJECT_PROP_PADDR_OTH_FULL                          0xDD2DU // OTH -> OTHER
#define MTP_OBJECT_PROP_PADDR_OTH_LINE1                         0xDD2EU
#define MTP_OBJECT_PROP_PADDR_OTH_LINE2                         0xDD2FU
#define MTP_OBJECT_PROP_PADDR_OTH_CITY                          0xDD30U
#define MTP_OBJECT_PROP_PADDR_OTH_REGION                        0xDD31U
#define MTP_OBJECT_PROP_PADDR_OTH_POSTAL_CODE                   0xDD32U
#define MTP_OBJECT_PROP_PADDR_OTH_COUNTRY                       0xDD33U
#define MTP_OBJECT_PROP_ORGANIZATION_NAME                       0xDD34U
#define MTP_OBJECT_PROP_PHONETIC_ORGANIZATION_NAME              0xDD35U
#define MTP_OBJECT_PROP_ROLE                                    0xDD36U
#define MTP_OBJECT_PROP_BIRTHDATE                               0xDD37U
#define MTP_OBJECT_PROP_MESSAGE_TO                              0xDD40U
#define MTP_OBJECT_PROP_MESSAGE_CC                              0xDD41U
#define MTP_OBJECT_PROP_MESSAGE_BCC                             0xDD42U
#define MTP_OBJECT_PROP_MESSAGE_READ                            0xDD43U
#define MTP_OBJECT_PROP_MESSAGE_RECEIVED_TIME                   0xDD44U
#define MTP_OBJECT_PROP_MESSAGE_SENDER                          0xDD45U
#define MTP_OBJECT_PROP_ACTIVITY_BEGIN_TIME                     0xDD50U
#define MTP_OBJECT_PROP_ACTIVITY_END_TIME                       0xDD51U
#define MTP_OBJECT_PROP_ACTIVITY_LOCATION                       0xDD52U
#define MTP_OBJECT_PROP_ACTIVITY_REQUIRED_ATTENDEES             0xDD54U
#define MTP_OBJECT_PROP_ACTIVITY_OPTIONAL_ATTENDEES             0xDD55U
#define MTP_OBJECT_PROP_ACTIVITY_RESOURCES                      0xDD56U
#define MTP_OBJECT_PROP_ACTIVITY_ACCEPTED                       0xDD57U
#define MTP_OBJECT_PROP_OWNER                                   0xDD5DU
#define MTP_OBJECT_PROP_EDITOR                                  0xDD5EU
#define MTP_OBJECT_PROP_WEBMASTER                               0xDD5FU
#define MTP_OBJECT_PROP_URL_SOURCE                              0xDD60U
#define MTP_OBJECT_PROP_URL_DESTINATION                         0xDD61U
#define MTP_OBJECT_PROP_TIME_BOOKMARK                           0xDD62U
#define MTP_OBJECT_PROP_OBJECT_BOOKMARK                         0xDD63U
#define MTP_OBJECT_PROP_BYTE_BOOKMARK                           0xDD64U
#define MTP_OBJECT_PROP_LAST_BUILD_DATE                         0xDD70U
#define MTP_OBJECT_PROP_TIME_TO_LIVE                            0xDD71U
#define MTP_OBJECT_PROP_MEDIA_GUID                              0xDD72U

/* Define MTP Operation Commands.  */

#define MTP_OC_UNDEFINED                                           0x1000U
#define MTP_OC_GET_DEVICE_INFO                                     0x1001U
#define MTP_OC_OPEN_SESSION                                        0x1002U
#define MTP_OC_CLOSE_SESSION                                       0x1003U
#define MTP_OC_GET_STORAGE_IDS                                     0x1004U
#define MTP_OC_GET_STORAGE_INFO                                    0x1005U
#define MTP_OC_GET_NUM_OBJECTS                                     0x1006U
#define MTP_OC_GET_OBJECT_HANDLES                                  0x1007U
#define MTP_OC_GET_OBJECT_INFO                                     0x1008U
#define MTP_OC_GET_OBJECT                                          0x1009U
#define MTP_OC_GET_THUMB                                           0x100AU
#define MTP_OC_DELETE_OBJECT                                       0x100BU
#define MTP_OC_SEND_OBJECT_INFO                                    0x100CU
#define MTP_OC_SEND_OBJECT                                         0x100DU
#define MTP_OC_INITIATE_CAPTURE                                    0x100EU
#define MTP_OC_FORMAT_STORE                                        0x100FU
#define MTP_OC_RESET_DEVICE                                        0x1010U
#define MTP_OC_SELF_TEST                                           0x1011U
#define MTP_OC_SET_OBJECT_PROTECTION                               0x1012U
#define MTP_OC_POWER_DOWN                                          0x1013U
#define MTP_OC_GET_DEVICE_PROP_DESC                                0x1014U
#define MTP_OC_GET_DEVICE_PROP_VALUE                               0x1015U
#define MTP_OC_SET_DEVICE_PROP_VALUE                               0x1016U
#define MTP_OC_RESET_DEVICE_PROP_VALUE                             0x1017U
#define MTP_OC_TERMINATE_OPEN_CAPTURE                              0x1018U
#define MTP_OC_MOVE_OBJECT                                         0x1019U
#define MTP_OC_COPY_OBJECT                                         0x101AU
#define MTP_OC_GET_PARTIAL_OBJECT                                  0x101BU
#define MTP_OC_INITIATE_OPEN_CAPTURE                               0x101CU
#define MTP_OC_GET_OBJECT_PROPS_SUPPORTED                       0x9801U
#define MTP_OC_GET_OBJECT_PROP_DESC                             0x9802U
#define MTP_OC_GET_OBJECT_PROP_VALUE                            0x9803U
#define MTP_OC_SET_OBJECT_PROP_VALUE                            0x9804U
#define MTP_OC_GET_OBJECT_PROP_LIST                             0x9805U
#define MTP_OC_GET_OBJECT_REFERENCES                            0x9810U
#define MTP_OC_SET_OBJECT_REFERENCES                            0x9811U
#define MTP_OC_AMBA_SET_DATETIME                                0x7701U
#define MTP_OC_AMBA_GET_DATETIME                                0x7702U

/* Define MTP Response Codes.  */
#define MTP_RC_UNDEFINED                                        0x2000U
#define MTP_RC_OK                                               0x2001U
#define MTP_RC_GENERAL_ERROR                                    0x2002U
#define MTP_RC_SESSION_NOT_OPEN                                 0x2003U
#define MTP_RC_INVALID_TRANSACTION_ID                           0x2004U
#define MTP_RC_OPERATION_NOT_SUPPORTED                          0x2005U
#define MTP_RC_PARAMETER_NOT_SUPPORTED                          0x2006U
#define MTP_RC_INCOMPLETE_TRANSFER                              0x2007U
#define MTP_RC_INVALID_STORAGE_ID                               0x2008U
#define MTP_RC_INVALID_OBJECT_HANDLE                            0x2009U
#define MTP_RC_DEVICE_PROP_NOT_SUPPORTED                        0x200AU
#define MTP_RC_INVALID_OBJECT_FORMAT_CODE                       0x200BU
#define MTP_RC_STORE_FULL                                       0x200CU
#define MTP_RC_OBJECT_WRITE_PROTECTED                           0x200DU
#define MTP_RC_STORE_READ_ONLY                                  0x200EU
#define MTP_RC_ACCESS_DENIED                                    0x200FU
#define MTP_RC_NO_THUMBNAIL_PRESENT                             0x2010U
#define MTP_RC_SELF_TEST_FAILED                                 0x2011U
#define MTP_RC_PARTIAL_DELETION                                 0x2012U
#define MTP_RC_STORE_NOT_AVAILABLE                              0x2013U
#define MTP_RC_FORMAT_UNSUPPORTED                               0x2014U
#define MTP_RC_NO_VALID_OBJECT_INFO                             0x2015U
#define MTP_RC_INVALID_CODE_FORMAT                              0x2016U
#define MTP_RC_UNKNOWN_VENDOR_CODE                              0x2017U
#define MTP_RC_CAPTURE_ALREADY_TERMINATED                       0x2018U
#define MTP_RC_DEVICE_BUSY                                      0x2019U
#define MTP_RC_INVALID_PARENT_OBJECT                            0x201AU
#define MTP_RC_INVALID_DEVICE_PROP_FORMAT                       0x201BU
#define MTP_RC_INVALID_DEVICE_PROP_VALUE                        0x201CU
#define MTP_RC_INVALID_PARAMETER                                0x201DU
#define MTP_RC_SESSION_ALREADY_OPENED                           0x201EU
#define MTP_RC_TRANSACTION_CANCELED                             0x201FU
#define MTP_RC_DESTINATION_UNSUPPORTED                          0x2020U
#define MTP_RC_OBJECT_ALREADY_OPENED                            0x2021U
#define MTP_RC_OBJECT_ALREADY_CLOSED                            0x2022U
#define MTP_RC_OBJECT_NOT_OPENED                                0x2023U
#define MTP_RC_ACCESS_ERROR                                     0x2024U
#define MTP_RC_OBJECT_SEEK_FAILED                               0x2025U
#define MTP_RC_INVALID_OBJECT_PROP_CODE                         0xA801U
#define MTP_RC_INVALID_OBJECT_PROP_FORMAT                       0xA802U
#define MTP_RC_INVALID_OBJECT_PROP_VALUE                        0xA803U
#define MTP_RC_INVALID_OBJECT_REFERENCE                         0xA804U
#define MTP_RC_INVALID_DATASET                                  0xA806U
#define MTP_RC_SPECIFICATION_BY_GROUP_UNSUPPORTED               0xA807U
#define MTP_RC_SPECIFICATION_BY_DEPTH_UNSUPPORTED               0xA808U
#define MTP_RC_OBJECT_TOO_LARGE                                 0xA809U
#define MTP_RC_OBJECT_PROP_NOT_SUPPORTED                        0xA80AU

/* Define MTP OBJECT PROPERTY DATASET.  */
#define MTP_OBJECT_PROPERTY_DATASET_CODE                        0x0000U
#define MTP_OBJECT_PROPERTY_DATASET_DATATYPE                    0x0002U
#define MTP_OBJECT_PROPERTY_DATASET_GETSET                      0x0004U
#define MTP_OBJECT_PROPERTY_DATASET_VALUE                       0x0005U

/* Define MTP types.  */
#define MTP_TYPES_INT8                                          0x0001U
#define MTP_TYPES_UINT8                                         0x0002U
#define MTP_TYPES_INT16                                         0x0003U
#define MTP_TYPES_UINT16                                        0x0004U
#define MTP_TYPES_INT32                                         0x0005U
#define MTP_TYPES_UINT32                                        0x0006U
#define MTP_TYPES_INT64                                         0x0007U
#define MTP_TYPES_UINT64                                        0x0008U
#define MTP_TYPES_INT128                                        0x0009U
#define MTP_TYPES_UINT128                                       0x000AU
#define MTP_TYPES_AINT8                                         0x4001U
#define MTP_TYPES_AUINT8                                        0x4002U
#define MTP_TYPES_AINT16                                        0x4003U
#define MTP_TYPES_AUINT16                                       0x4004U
#define MTP_TYPES_AINT32                                        0x4005U
#define MTP_TYPES_AUINT32                                       0x4006U
#define MTP_TYPES_AINT64                                        0x4007U
#define MTP_TYPES_AUINT64                                       0x4008U
#define MTP_TYPES_AINT128                                       0x4009U
#define MTP_TYPES_AUINT128                                      0x400AU
#define MTP_TYPES_STR                                           0xFFFFU

/* Define MTP Event Codes.  */

#define MTP_EC_UNDEFINED                                        0x4000U
#define MTP_EC_CANCEL_TRANSACTION                               0x4001U
#define MTP_EC_OBJECT_ADDED                                     0x4002U
#define MTP_EC_OBJECT_REMOVED                                   0x4003U
#define MTP_EC_STORE_ADDED                                      0x4004U
#define MTP_EC_STORE_REMOVED                                    0x4005U
#define MTP_EC_DEVICE_PROP_CHANGED                              0x4006U
#define MTP_EC_OBJECT_INFO_CHANGED                              0x4007U
#define MTP_EC_DEVICE_INFO_CHANGED                              0x4008U
#define MTP_EC_REQUEST_OBJECT_TRANSFER                          0x4009U
#define MTP_EC_STORE_FULL                                       0x400AU
#define MTP_EC_DEVICE_RESET                                     0x400BU
#define MTP_EC_STORAGE_INFO_CHANGED                             0x400CU
#define MTP_EC_CAPTURE_COMPLETE                                 0x400DU
#define MTP_EC_UNREPORTED_STATUS                                0x400EU
#define MTP_EC_VENDOR_AMBA_TEST                                 0xC002U


#endif
