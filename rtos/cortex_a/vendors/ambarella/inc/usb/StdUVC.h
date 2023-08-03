/**
 *  @file StdUVC.h
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
 *  @details Standard USB Video Class header file.
 */

#ifndef STD_USB_VIDEO_CLASS_H
#define STD_USB_VIDEO_CLASS_H

// ----------------------------------------
// UVC device class
// ----------------------------------------
#define USB_DEVICE_CLASS_CODE_UVC     0xEF
#define USB_DEVICE_SUBCLASS_CODE_UVC  0x02
#define USB_DEVICE_PROTOCOL_CODE_UVC  0x01

// UVC Descriptor Length
#define UVC_DESC_LENGTH_VC_INTF         0x0DU
#define UVC_DESC_LENGTH_OUTPUT_TERMINAL 0x09U
#define UVC_DESC_LENGTH_INPUT_TERMINAL  0x12U
#define UVC_DESC_LENGTH_PROCESSING_UNIT 0x0BU
#define UVC_DESC_LENGTH_ENCODING_UNIT   0x0DU
#define UVC_DESC_LENGTH_CS_ENDPOINT     0x05U
#define UVC_DESC_LENGTH_VS_INPUT_HEADER 0x0EU
#define UVC_DESC_LENGTH_VS_MJPEG_FORMAT 0x0BU
#define UVC_DESC_LENGTH_VS_UNCOMPRESSED_FORMAT 0x1BU
#define UVC_DESC_LENGTH_VS_MJPEG_FRAME  0x26U
#define UVC_DESC_LENGTH_VS_H264_FORMAT  0x34U
#define UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT 0x1CU


// UVC Descriptor Type
#define UVC_DESC_TYPE_CS_UNDEFINED                    0x20U
#define UVC_DESC_TYPE_CS_DEVICE                       0x21U
#define UVC_DESC_TYPE_CS_CONFIGURATION                0x22U
#define UVC_DESC_TYPE_CS_STRING                       0x23U
#define UVC_DESC_TYPE_CS_INTF                         0x24U
#define UVC_DESC_TYPE_CS_ENDPOINT                     0x25U

// UVC Descriptor SubType
#define UVC_DESC_SUBTYPE_VC_HEADER          0x01U
#define UVC_DESC_SUBTYPE_VC_INPUT_TERMINAL  0x02U
#define UVC_DESC_SUBTYPE_VC_OUTPUT_TERMINAL 0x03U
#define UVC_DESC_SUBTYPE_VC_SELECTOR_UNIT   0x04U
#define UVC_DESC_SUBTYPE_VC_PROCESSING_UNIT 0x05U
#define UVC_DESC_SUBTYPE_VC_EXTENSION_UNIT  0x06U
#define UVC_DESC_SUBTYPE_VC_ENCODING_UNIT    0x07U

#define UVC_DESC_SUBTYPE_VS_INPUT_HEADER            0x01U
#define UVC_DESC_SUBTYPE_VS_OUTPUT_HEADER           0x02U
#define UVC_DESC_SUBTYPE_VS_STILL_IMAGE_FRAME       0x03U
#define UVC_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED     0x04U
#define UVC_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED      0x05U
#define UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG            0x06U
#define UVC_DESC_SUBTYPE_VS_FRAME_MJPEG             0x07U
#define UVC_DESC_SUBTYPE_VS_FORMAT_MPEG2TS          0x0AU
#define UVC_DESC_SUBTYPE_VS_FORMAT_DV               0x0CU
#define UVC_DESC_SUBTYPE_VS_COLORFORMAT             0x0DU
#define UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED      0x10U
#define UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED       0x11U
#define UVC_DESC_SUBTYPE_VS_FORMAT_STREAM_BASED     0x12U
#define UVC_DESC_SUBTYPE_VS_FORMAT_H264             0x13U
#define UVC_DESC_SUBTYPE_VS_FRAME_H264              0x14U
//#define UVC_DESC_SUBTYPE_VS_FORMAT_H264_SIMULCAST   0x15U
#define UVC_DESC_SUBTYPE_VS_FORMAT_VP8              0x16U
#define UVC_DESC_SUBTYPE_VS_FRAME_VP8               0x17U
#define UVC_DESC_SUBTYPE_VS_FORMAT_VP8_SIMULCAST    0x18U

// Interface class code
#define UVC_INTF_CLASS_CODE_VIDEO                   0x0EU

// Interface Subclass code
#define UVC_INTF_SUBCLASS_CODE_VIDEO_CONTROL        0x01U
#define UVC_INTF_SUBCLASS_CODE_VIDEO_STREAMING      0x02U
#define UVC_INTF_SUBCLASS_CODE_IAD                  0x03U

// Interface protocol code
#define UVC_INTF_PROTOCOL_CODE_UNDEFINE             0x00
#define UVC_INTF_PROTOCOL_CODE_15                   0x01U

// UVC Endpoint Subtype
#define UVC_ENDPOINT_DESC_SUBTYPE_UNDEFINED 0x00
#define UVC_ENDPOINT_DESC_SUBTYPE_GENERAL   0x01
#define UVC_ENDPOINT_DESC_SUBTYPE_ENDPOINT  0x02
#define UVC_ENDPOINT_DESC_SUBTYPE_INTERRUPT 0x03

// UVC Terminal Type
#define UVC_TERMINAL_TYPE_UNDEFINED 0x0100
#define UVC_TERMINAL_TYPE_STREAMING 0x0101
#define UVC_TERMINAL_TYPE_VENDOR_SPECIFIC 0x01FF

// UVC Input Terminal Type
#define UVC_INPUT_TERMINAL_TYPE_VENDOR_SPECIFIC      0x0200U
#define UVC_INPUT_TERMINAL_TYPE_CAMERA               0x0201U
#define UVC_INPUT_TERMINAL_TYPE_EDIA_TRANSPORT_INPUT 0x0202U

// UVC Output Terminal Type
#define UVC_OUTPUT_TERMINAL_TYPE_VENDOR_SPECIFIC     0x0300
#define UVC_OUTPUT_TERMINAL_TYPE_DISPLAY             0x0301
#define UVC_OUTPUT_TERMINAL_TYPE_MEDIA_TRANSPORT     0x0302

// CT control mask
#define UVC_CT_CONTROL_SCANNING_MODE_MASK            0x01U
#define UVC_CT_CONTROL_AUTO_EXPOSURE_MODE_MASK        0x02U
#define UVC_CT_CONTROL_AUDO_EXPOSURE_PRIORITY_MASK    0x04U
#define UVC_CT_CONTROL_EXPOSURE_TIME_ABSOLUTE_MASK    0x08U
#define UVC_CT_CONTROL_EXPOSURE_TIME_RELATIVE_MASK    0x10U
#define UVC_CT_CONTROL_FOCUS_ABSOLUTE_MASK            0x20U
#define UVC_CT_CONTROL_FOCUS_RELATIVE_MASK            0x40U
#define UVC_CT_CONTROL_IRIS_ABSOLUTE_MASK            0x80U
#define UVC_CT_CONTROL_IRIS_RELATIVE_MASK            0x100U
#define UVC_CT_CONTROL_ZOOM_ABSOLUTE_MASK            0x200U
#define UVC_CT_CONTROL_ZOOM_RELATIVE_MASK            0x400U
#define UVC_CT_CONTROL_PAN_TILE_ABSOLUTE_MASK        0x800U
#define UVC_CT_CONTROL_PAN_TILT_RELATIVE_MASK        0x1000U
#define UVC_CT_CONTROL_ROLL_ABSOLUTE_MASK            0x2000U
#define UVC_CT_CONTROL_ROLL_RELATIVE_MASK            0x4000U
#define UVC_CT_CONTROL_RSVD1_MASK                    0x8000U
#define UVC_CT_CONTROL_RSVD2_MASK                    0x10000U
#define UVC_CT_CONTROL_FOCUS_AUTO_MASK                0x20000U
#define UVC_CT_CONTROL_PRIVACY_MASK                    0x40000U
#define UVC_CT_CONTROL_RSVD3_MASK                    0x80000U
#define UVC_CT_CONTROL_RSVD4_MASK                    0x100000U
#define UVC_CT_CONTROL_RSVD5_MASK                    0x200000U
#define UVC_CT_CONTROL_RSVD6_MASK                    0x400000U
#define UVC_CT_CONTROL_RSVD7_MASK                    0x800000U

// PU control mask
#define UVC_PU_CONTROL_BRIGHTNESS_MASK               0x01U
#define UVC_PU_CONTROL_CONTRAST_MASK                 0x02U
#define UVC_PU_CONTROL_HUE_MASK                      0x04U
#define UVC_PU_CONTROL_SATURATION_MASK               0x08U
#define UVC_PU_CONTROL_SHARPNESS_MASK                0x10U
#define UVC_PU_CONTROL_GAMMA_MASK                    0x20U
#define UVC_PU_CONTROL_WB_TEMPERATURE_MASK           0x40U
#define UVC_PU_CONTROL_WB_COMPONENT_MASK             0x80U
#define UVC_PU_CONTROL_BACKLIGHT_COMPENSATION_MASK   0x100U
#define UVC_PU_CONTROL_GAIN_MASK                     0x200U
#define UVC_PU_CONTROL_POWER_LINE_FREQUENCY_MASK     0x400U
#define UVC_PU_CONTROL_HUE_AUTO_MASK                 0x800U
#define UVC_PU_CONTROL_WB_TEMPERATURE_AUTO_MASK      0x1000U
#define UVC_PU_CONTROL_WB_COMPONENT_AUTO_MASK        0x2000U
#define UVC_PU_CONTROL_DIGITAL_MULT_MASK             0x4000U // UVC_PU_CONTROL_DIGITAL_MULT_MASK -> UVC_PU_CONTROL_DIGITAL_MULTIPLIER_MASK
#define UVC_PU_CONTROL_DIGITAL_MULT_LIMIT_MASK       0x8000U // UVC_PU_CONTROL_DIGITAL_MULT_LIMIT_MASK -> UVC_PU_CONTROL_DIGITAL_MULTIPLIERLIMIT_MASK

/* VideoControl Interface Control Selectors */
#define UVC_VC_CONTROL_UNDEFINED                0x00U
#define UVC_VC_VIDEO_POWER_MODE_CONTROL         0x01U
#define UVC_VC_REQUEST_ERROR_CODE_CONTROL       0x02U

/* VideoStreaming Interface Control Selectors */
#define UVC_VS_CONTROL_UNDEFINED                0x00U
#define UVC_VS_PROBE_CONTROL                    0x01U
#define UVC_VS_COMMIT_CONTROL                   0x02U
#define UVC_VS_STILL_PROBE_CONTROL              0x03U
#define UVC_VS_STILL_COMMIT_CONTROL             0x04U
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL      0x05U
#define UVC_VS_STREAM_ERROR_CODE_CONTROL        0x06U
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL       0x07U
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL     0x08U
#define UVC_VS_SYNCH_DELAY_CONTROL              0x09U

/* UVC Camera Terminal Control Selectors */
#define UVC_CT_CONTROL_UNDEFINED                  0x00U
#define UVC_CT_SCANNING_MODE_CONTROL              0x01U
#define UVC_CT_AE_MODE_CONTROL                    0x02U
#define UVC_CT_AE_PRIORITY_CONTROL                0x03U
#define UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL     0x04U
#define UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL     0x05U
#define UVC_CT_FOCUS_ABSOLUTE_CONTROL             0x06U
#define UVC_CT_FOCUS_RELATIVE_CONTROL             0x07U
#define UVC_CT_FOCUS_AUTO_CONTROL                 0x08U
#define UVC_CT_IRIS_ABSOLUTE_CONTROL              0x09U
#define UVC_CT_IRIS_RELATIVE_CONTROL              0x0AU
#define UVC_CT_ZOOM_ABSOLUTE_CONTROL              0x0BU
#define UVC_CT_ZOOM_RELATIVE_CONTROL              0x0CU
#define UVC_CT_PANTILT_ABSOLUTE_CONTROL           0x0DU
#define UVC_CT_PANTILT_RELATIVE_CONTROL           0x0EU
#define UVC_CT_ROLL_ABSOLUTE_CONTROL              0x0FU
#define UVC_CT_ROLL_RELATIVE_CONTROL              0x10U
#define UVC_CT_PRIVACY_CONTROL                    0x11U
#define UVC_CT_FOCUS_SIMPLE_CONTROL               0x12U
#define UVC_CT_WINDOW_CONTROL                     0x13U
#define UVC_CT_REGION_OF_INTEREST_CONTROL         0x14U
#define UVC_CT_NUMBER                             0x15U

/* UVC Processing Unit Control Selectors */
#define UVC_PU_CONTROL_UNDEFINED                               0x00U
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL                  0x01U
#define UVC_PU_BRIGHTNESS_CONTROL                              0x02U
#define UVC_PU_CONTRAST_CONTROL                                0x03U
#define UVC_PU_GAIN_CONTROL                                    0x04U
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL                    0x05U
#define UVC_PU_HUE_CONTROL                                     0x06U
#define UVC_PU_SATURATION_CONTROL                              0x07U
#define UVC_PU_SHARPNESS_CONTROL                               0x08U
#define UVC_PU_GAMMA_CONTROL                                   0x09U
#define UVC_PU_WHITE_BALANCE_TEMP_CONTROL                      0x0AU // UVC_PU_WHITE_BALANCE_TEMP_CONTROL -> UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL
#define UVC_PU_WHITE_BALANCE_TEMP_AUTO_CONTROL                 0x0BU // UVC_PU_WHITE_BALANCE_TEMP_AUTO_CONTROL-> UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL
#define UVC_PU_WHITE_BALANCE_COMP_CONTROL                      0x0CU // UVC_PU_WHITE_BALANCE_COMP_CONTROL -> UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL
#define UVC_PU_WHITE_BALANCE_COMP_AUTO_CONTROL                 0x0DU // UVC_PU_WHITE_BALANCE_COMP_AUTO_CONTROL -> UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL                      0x0EU
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL                0x0FU
#define UVC_PU_HUE_AUTO_CONTROL                                0x10U
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL                   0x11U
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL                      0x12U
#define UVC_PU_PU_CONTRAST_AUTO_CONTROL                        0x13U
#define UVC_PU_NUMBER                                          0x14U


/* UVC 1.1 Extension Unit Control Selectors */
#define UVC_XU_H264_CONTROL_UNDEFINED           0x00U
#define UVC_XU_H264_CONFIG_PROBE                0x01U
#define UVC_XU_H264_CONFIG_COMMIT               0x02U
#define UVC_XU_H264_RATE_CONTROL_MODE           0x03U
#define UVC_XU_H264_TEMPORAL_SCALE_MODE         0x04U
#define UVC_XU_H264_SPATIAL_SCALE_MODE          0x05U
#define UVC_XU_H264_SNR_SCALE_MODE              0x06U
#define UVC_XU_H264_LTR_BUFFER_SIZE_CONTROL     0x07U
#define UVC_XU_H264_LTR_PICTURE_CONTROL         0x08U
#define UVC_XU_H264_PICTURE_TYPE_CONTROL        0x09U
#define UVC_XU_H264_VERSION                     0x0AU
#define UVC_XU_H264_ENCODER_RESET               0x0BU
#define UVC_XU_H264_FRAMERATE_CONFIG            0x0CU
#define UVC_XU_H264_VIDEO_ADVANCE_CONFIG        0x0DU
#define UVC_XU_H264_BITRATE_LAYERS              0x0EU
#define UVC_XU_H264_QP_STEPS_LAYERS             0x0FU
#define UVC_XU_H264_NUMBER                      0x10U

/* bRateControlMode defines */
#define RATECONTROL_CBR           0x01
#define RATECONTROL_VBR           0x02
#define RATECONTROL_CONST_QP      0x03
#define RATECONTROL_FIXED_FRM_FLG 0x10

/* bStreamFormat defines */
#define STREAMFORMAT_ANNEXB       0x00
#define STREAMFORMAT_NAL          0x01

/* bEntropyCABAC defines */
#define ENTROPY_CAVLC             0x00
#define ENTROPY_CABAC             0x01

/* bTimingstamp defines */
#define TIMESTAMP_SEI_DISABLE     0x00
#define TIMESTAMP_SEI_ENABLE      0x01

/* bPreviewFlipped defines */
#define PREFLIPPED_DISABLE        0x00
#define PREFLIPPED_HORIZONTAL     0x01

/* wPictureType defines */
#define PICTURE_TYPE_IFRAME     0x0000 //Generate an IFRAME
#define PICTURE_TYPE_IDR        0x0001 //Generate an IDR
#define PICTURE_TYPE_IDR_FULL    0x0002 //Generate an IDR frame with new SPS and PPS

/* bStreamMuxOption defines */
#define STREAMMUX_H264          ((1U /*<< 0*/) | (1U << 1U))
#define STREAMMUX_YUY2          ((1U /*<< 0*/) | (1U << 2U))
#define STREAMMUX_YUYV          ((1U /*<< 0*/) | (1U << 2U))
#define STREAMMUX_NV12          ((1U /*<< 0*/) | (1U << 3U))

/* UVC 1.5 Encoding Unit Control Selectors */
#define UVC_EU_CONTROL_UNDEFINED                0x00U
#define UVC_EU_SELECT_LAYER_CONTROL             0x01U
#define UVC_EU_PROFILE_TOOLSET_CONTROL          0x02U
#define UVC_EU_VIDEO_RESOLUTION_CONTROL         0x03U
#define UVC_EU_MIN_FRAME_INTERVAL_CONTROL       0x04U
#define UVC_EU_SLICE_MODE_CONTROL               0x05U
#define UVC_EU_RATE_CONTROL_MODE_CONTROL        0x06U
#define UVC_EU_AVERAGE_BITRATE_CONTROL          0x07U
#define UVC_EU_CPB_SIZE_CONTROL                 0x08U
#define UVC_EU_PEAK_BIT_RATE_CONTROL            0x09U
#define UVC_EU_QUANTIZATION_PARAMS_CONTROL      0x0AU
#define UVC_EU_SYNC_REF_FRAME_CONTROL           0x0BU
#define UVC_EU_LTR_BUFFER_CONTROL               0x0CU
#define UVC_EU_LTR_PICTURE_CONTROL              0x0DU
#define UVC_EU_LTR_VALIDATION_CONTROL           0x0EU
#define UVC_EU_LEVEL_IDC_LIMIT_CONTROL          0x0FU
#define UVC_EU_SEI_PAYLOADTYPE_CONTROL          0x10U
#define UVC_EU_QP_RANGE_CONTROL                 0x11U
#define UVC_EU_PRIORITY_CONTROL                 0x12U
#define UVC_EU_START_OR_STOP_LAYER_CONTROL      0x13U
#define UVC_EU_ERROR_RESILIENCY_CONTROL         0x14U
#define UVC_EU_NUMBER                           0x15U


/* Video Class-Specific Request Code */
#define UVC_SET_CUR      0x01U
#define UVC_SET_CUR_ALL  0x11U
#define UVC_GET_CUR      0x81U
#define UVC_GET_MIN      0x82U
#define UVC_GET_MAX      0x83U
#define UVC_GET_RES      0x84U
#define UVC_GET_LEN      0x85U
#define UVC_GET_INFO     0x86U
#define UVC_GET_DEF      0x87U
#define GET_CUR_ALL      0x91U
#define GET_MIN_ALL      0x92U
#define GET_MAX_ALL      0x93U
#define GET_RES_ALL      0x94U
#define GET_DEF_ALL      0x97U

/* VideoControl Error Code */
#define UVC_VC_ER_CD_NO_ERROR            0x00U
#define UVC_VC_ER_CD_NOT_READY           0x01U
#define UVC_VC_ER_CD_WRONG_STATE         0x02U
#define UVC_VC_ER_CD_POWER               0x03U
#define UVC_VC_ER_CD_OUT_OF_RANGE        0x04U
#define UVC_VC_ER_CD_INVALID_UNIT        0x05U
#define UVC_VC_ER_CD_INVALID_CONTROL     0x06U
#define UVC_VC_ER_CD_INVALID_REQUEST     0x07U
#define UVC_VC_ER_CD_UNKNOWN             0xFFU

#endif
