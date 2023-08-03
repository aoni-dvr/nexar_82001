/**
 *  @file ux_device_class_uvc_probe_control.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details probe control functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

static uvcd_host_config_t uvc_host_config    = { 0 };
static uvcd_event_callback_t event_callbacks = { NULL };

static uvcd_probe_control_t uvc_probe_control =
{
    0x0000,                  /* bmHint,
                                D0: dwFrameInterval
                                D1: wKeyFrameRate
                                D2: wPFrameRate
                                D3: wCompQuality
                                D4: wCompWindowSize      */
    UVC_FORMAT_INDEX_MJPEG,  /* bFormatIndex */
    UVC_FRAME_INDEX_320x240, /* bFrameIndex */
    0x00051615,              /* dwFrameInterval, 30 fps */
    0x0000,                  /* wKeyFrameRate */
    0x0000,                  /* wPFrameRate */
    5000,                    /* wCompQuality */
    0x0000,                  /* wCompWindowSize */
    0x0000,                  /* wDelay */
    0xE1000,                 /* dwMaxVideoFrameSize */
    512,                     /* dwMaxPayloadTransferSize */
};

static uvcd_still_probe_control_t uvc_still_probe_control =
{
    1,          /* bFormatIndex */
    1,          /* bFrameIndex */
    0,          /* bCompressionIndex */
    1310720,    /* dwMaxVideoFrameSize */
    8192,       /* dwMaxPayloadTransferSize */
};

static void dbg(const char *str)
{
	_ux_utility_print_uint5(str, 0, 0, 0, 0, 0);
}

uvcd_probe_control_t *uvcd_get_probe_control(void)
{
    return &uvc_probe_control;
}


void uvcd_copy_probe_control(const uvcd_probe_control_t *control, UINT8 *out)
{
    _ux_utility_short_put(out, control->bmHint);
    out[2] = control->bFormatIndex;
    out[3] = control->bFrameIndex;
    _ux_utility_long_put(&out[4], control->dwFrameInterval);
    _ux_utility_short_put(&out[8], control->wKeyFrameRate);
    _ux_utility_short_put(&out[10], control->wPFrameRate);
    _ux_utility_short_put(&out[12], control->wCompQuality);
    _ux_utility_short_put(&out[14], control->wCompWindowSize);
    _ux_utility_short_put(&out[16], control->wDelay);
    _ux_utility_long_put(&out[18], control->dwMaxVideoFrameSize);
    _ux_utility_long_put(&out[22], control->dwMaxPayloadTransferSize);
}

static void uvcd_copy_still_probe_control(const uvcd_still_probe_control_t *control, UINT8 *out)
{
    out[0] = control->bFormatIndex;
    out[1] = control->bFrameIndex;
    out[2] = control->bCompressionIndex;
    _ux_utility_long_put(&out[3], control->dwMaxVideoFrameSize);
    _ux_utility_long_put(&out[7], control->dwMaxPayloadTransferSize);
}

void uvcd_decode_probe_control(UINT8 *data, uvcd_probe_control_t *control)
{
    control->bmHint                   = (UINT16)_ux_utility_short_get(data);
    control->bFormatIndex             = data[2];
    control->bFrameIndex              = data[3];
    control->dwFrameInterval          = _ux_utility_long_get(&data[4]);
    control->wKeyFrameRate            = (UINT16)_ux_utility_short_get(&data[8]);
    control->wPFrameRate              = (UINT16)_ux_utility_short_get(&data[10]);
    control->wCompQuality             = (UINT16)_ux_utility_short_get(&data[12]);
    control->wCompWindowSize          = (UINT16)_ux_utility_short_get(&data[14]);
    control->wDelay                   = (UINT16)_ux_utility_short_get(&data[16]);
    control->dwMaxVideoFrameSize      = _ux_utility_long_get(&data[18]);
    control->dwMaxPayloadTransferSize = _ux_utility_long_get(&data[22]);
}

void uvcd_decode_still_probe_control(UINT8 *data, uvcd_still_probe_control_t *control)
{
    control->bFormatIndex             = data[0];
    control->bFrameIndex              = data[1];
    control->bCompressionIndex        = data[2];
    control->dwMaxVideoFrameSize      = _ux_utility_long_get(&data[3]);
    control->dwMaxPayloadTransferSize = _ux_utility_long_get(&data[7]);
}

static void uvc_parse_mjpeg_resolution(UINT8 *desc, ULONG desc_size, UINT8 format_idx, UINT8 frame_idx)
{
    ULONG idx             = 0;
    UINT8 len             = 0;
    UINT8 type            = 0;
    UINT8 format_desc_idx = 0;
    UINT8 frame_desc_idx  = 0;
    UINT8 subtype         = 0;
    UINT8 is_break        = 0;

    while (idx < desc_size) {
        if (is_break == 1U) {
            break;
        }
        len  = desc[idx];
        type = desc[idx + 1U];

        if (format_desc_idx == format_idx) {
            // matched format index, found frame descriptor
            subtype = desc[idx + 2U];
            if ((type == UVC_DESC_TYPE_CS_INTF) && (subtype == UVC_DESC_SUBTYPE_VS_FRAME_MJPEG)) {
                frame_desc_idx = desc[idx + 3U];
                if (frame_idx == frame_desc_idx) {
                    UINT32 width  = _ux_utility_short_get(&desc[idx + 5U]);
                    UINT32 height = _ux_utility_short_get(&desc[idx + 7U]);
                    _ux_utility_print_uint5("[UVC] format %d, frame %d ==> %dx%d", format_idx, frame_idx, width, height, 0);
                    uvc_host_config.height = height;
                    uvc_host_config.width  = width;
                    uvc_host_config.format = UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG;
                    is_break               = 1;
                    continue;
                }
            }

            if ((type == UVC_DESC_TYPE_CS_INTF) && (subtype == UVC_DESC_SUBTYPE_VS_FRAME_FRAME_BASED)) {
                frame_desc_idx = desc[idx + 3U];
                if (frame_idx == frame_desc_idx) {
                    UINT32 width  = _ux_utility_short_get(&desc[idx + 5U]);
                    UINT32 height = _ux_utility_short_get(&desc[idx + 7U]);
                    _ux_utility_print_uint5("[UVC] format %d, frame %d ==> %dx%d", format_idx, frame_idx, width, height, 0);
                    uvc_host_config.height = height;
                    uvc_host_config.width  = width;
                    uvc_host_config.format = UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED;
                    is_break               = 1;
                    continue;
                }
            }

            if ((type == UVC_DESC_TYPE_CS_INTF) && (subtype == UVC_DESC_SUBTYPE_VS_FRAME_H264)) {
                frame_desc_idx = desc[idx + 3U];
                if (frame_idx == frame_desc_idx) {
                    UINT32 width  = _ux_utility_short_get(&desc[idx + 4U]);
                    UINT32 height = _ux_utility_short_get(&desc[idx + 6U]);
                    _ux_utility_print_uint5("[UVC] format %d, frame %d ==> %dx%d", format_idx, frame_idx, width, height, 0);
                    uvc_host_config.height = height;
                    uvc_host_config.width  = width;
                    uvc_host_config.format = UVC_DESC_SUBTYPE_VS_FORMAT_H264;
                    is_break               = 1;
                    continue;
                }
            }

            if ((type == UVC_DESC_TYPE_CS_INTF) && (subtype == UVC_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED)) {
                frame_desc_idx = desc[idx + 3U];
                if (frame_idx == frame_desc_idx) {
                    UINT32 width  = _ux_utility_short_get(&desc[idx + 5U]);
                    UINT32 height = _ux_utility_short_get(&desc[idx + 7U]);
                    _ux_utility_print_uint5("[UVC] format %d, frame %d ==> %dx%d", format_idx, frame_idx, width, height, 0);
                    uvc_host_config.height = height;
                    uvc_host_config.width  = width;
                    uvc_host_config.format = UVC_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED;
                    is_break               = 1;
                    continue;
                }
            }
        } else {
            // find Format Descriptor first
            if ((type == UVC_DESC_TYPE_CS_INTF) && (len == UVC_DESC_LENGTH_VS_MJPEG_FORMAT)) {
                subtype = desc[idx + 2U];
                if (subtype == UVC_DESC_SUBTYPE_VS_FORMAT_MJPEG) {
                    format_desc_idx = desc[idx + 3U];
                }
            }

            if ((type == UVC_DESC_TYPE_CS_INTF) && (len == UVC_DESC_LENGTH_VS_FRAME_BASED_FORMAT)) {
                subtype = desc[idx + 2U];
                if (subtype == UVC_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED) {
                    format_desc_idx = desc[idx + 3U];
                }
            }
            if ((type == UVC_DESC_TYPE_CS_INTF) && (len == UVC_DESC_LENGTH_VS_H264_FORMAT)) {
                subtype = desc[idx + 2U];
                if (subtype == UVC_DESC_SUBTYPE_VS_FORMAT_H264) {
                    format_desc_idx = desc[idx + 3U];
                }
            }
            if ((type == UVC_DESC_TYPE_CS_INTF) && (len == UVC_DESC_LENGTH_VS_UNCOMPRESSED_FORMAT)) {
                subtype = desc[idx + 2U];
                if (subtype == UVC_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED) {
                    format_desc_idx = desc[idx + 3U];
                }
            }
        }
        idx += len;
    }
}

VOID uvcd_config_mjpeg_resolution(UINT8 format_idx, UINT8 frame_idx)
{
    UINT8 *desc_ptr = NULL;
    ULONG desc_len  = 0;

    if ((_ux_system_slave->ux_system_slave_speed == (ULONG)UX_SUPER_SPEED_DEVICE)) {
        desc_ptr = _ux_system_slave->device_framework_super_speed;
        desc_len = _ux_system_slave->device_framework_length_super_speed;
    } else if (_ux_system_slave->ux_system_slave_speed == (ULONG)UX_HIGH_SPEED_DEVICE) {
        desc_ptr = _ux_system_slave->device_framework_high_speed;
        desc_len = _ux_system_slave->device_framework_length_high_speed;
    } else {
        desc_ptr = _ux_system_slave->device_framework_full_speed;
        desc_len = _ux_system_slave->device_framework_length_full_speed;
    }

    uvc_parse_mjpeg_resolution(desc_ptr, desc_len, format_idx, frame_idx);
}

UINT32 uvcd_get_mjpeg_quality(void)
{
    return uvc_host_config.mjpeg_quality;
}

UINT32 uvcd_set_mjpeg_quality(UINT32 quality)
{
    uvc_host_config.mjpeg_quality = quality;
    return UX_SUCCESS;
}

UINT32 uvcd_get_host_config(uvcd_host_config_t *config)
{
    _ux_utility_memory_copy(config, &uvc_host_config, sizeof(uvcd_host_config_t));
    return UX_SUCCESS;
}

UINT32 uvcd_get_probe_max_payload_xfer_size(void)
{
    return uvc_probe_control.dwMaxPayloadTransferSize;
}

static UINT32 uvc_set_max_payload = 0;
void uvcd_set_probe_max_payload_xfer_size(UINT32 size)
{
    if (size > 0U) {
        uvc_set_max_payload                        = 1;
        uvc_probe_control.dwMaxPayloadTransferSize = size;
    } else {
        uvc_set_max_payload                        = 0;
        uvc_probe_control.dwMaxPayloadTransferSize = 0;
    }
}

VOID uvcd_config_probe_control(UCHAR *data_pointer, ULONG request_length)
{
    uvcd_probe_control_t c;

    (VOID)request_length;

    if (data_pointer != NULL) {
        uvcd_decode_probe_control(data_pointer, &c);

        _ux_utility_print("UVC_Probe_Control from Host: ");
        _ux_utility_print_uint5("\t bmHint : 0x%X", c.bmHint, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFormatIndex : 0x%X", c.bFormatIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFrameIndex : 0x%X", c.bFrameIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwFrameInterval : %d", c.dwFrameInterval, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wKeyFrameRate : 0x%X", c.wKeyFrameRate, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wPFrameRate : 0x%X", c.wPFrameRate, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wCompQuality : %d", c.wCompQuality, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wCompWindowSize : 0x%X", c.wCompWindowSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wDelay : 0x%X", c.wDelay, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxVideoFrameSize : 0x%X", c.dwMaxVideoFrameSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxPayloadTransferSize : 0x%X", c.dwMaxPayloadTransferSize, 0, 0, 0, 0);

        if (c.bFormatIndex != 0U) {
            uvc_probe_control.bFormatIndex = c.bFormatIndex;
        }

        if (c.bFrameIndex != 0U) {
            uvc_probe_control.bFrameIndex = c.bFrameIndex;
        }

        /* config frame rate */
        if (c.dwFrameInterval != 0U) {
            if (uvc_probe_control.dwFrameInterval != c.dwFrameInterval) {
                uvc_probe_control.dwFrameInterval = c.dwFrameInterval;
            }
        }

        if (uvc_probe_control.wCompQuality != c.wCompQuality) {
            UINT32 mjpeg_quality;
            uvc_probe_control.wCompQuality = c.wCompQuality;
            mjpeg_quality                  = (UINT32)uvc_probe_control.wCompQuality / 100U;
            if (mjpeg_quality < 10U) {
                _ux_utility_print_uint5("[Warning] config_uvc_probe_control(): mjpeg quality(%d) < 10, set to 10.", mjpeg_quality, 0, 0, 0, 0);
                mjpeg_quality = 10;
            }
            if (mjpeg_quality > 90U) {
                _ux_utility_print_uint5("[Warning] config_uvc_probe_control(): mjpeg quality(%d) > 90, set to 90.", mjpeg_quality, 0, 0, 0, 0);
                mjpeg_quality = 90U;
            }
            if (uvcd_set_mjpeg_quality(mjpeg_quality) != 0U) {
                // ignore error
            }
        }

        if (c.dwMaxVideoFrameSize != 0U) {
            uvc_probe_control.dwMaxVideoFrameSize = c.dwMaxVideoFrameSize;
        }

        /* dwMaxPayloadTransferSize is only set by device. */
        // The value is determined in uvc_probe_control.dwMaxPayloadTransferSize.
        // Application can change current value through probe control API before UVC starts.
        if (uvc_set_max_payload == 0U) {
            if ((_ux_system_slave->ux_system_slave_speed == (ULONG)UX_SUPER_SPEED_DEVICE)) {
                // (Todo) set the dwMaxPayloadTransferSize to 1MB for Super Speed Bulk pipe.
                uvc_probe_control.dwMaxPayloadTransferSize = 1024 * 1024;
            } else if (_ux_system_slave->ux_system_slave_speed == (ULONG)UX_HIGH_SPEED_DEVICE) {
                // set the dwMaxPayloadTransferSize to 1MB for High Speed Bulk pipe.
                uvc_probe_control.dwMaxPayloadTransferSize = 1024 * 1024;
            } else {
                // set the dwMaxPayloadTransferSize to 200KB for Full Speed Bulk pipe
                // OHCI only has 256KB buffer, if the USB packet is larger than 256KB, the HOST(Windows) will
                // crash(BSOD).
                // UHCI doesn't have this limitation.
                uvc_probe_control.dwMaxPayloadTransferSize = 1024 * 200;
            }
        }

        uvc_host_config.framerate = 10000000U / uvc_probe_control.dwFrameInterval;

        _ux_utility_print("UVC_Probe_Control current : ");
        _ux_utility_print_uint5("\t bmHint : 0x%X", uvc_probe_control.bmHint, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFormatIndex : 0x%X", uvc_probe_control.bFormatIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFrameIndex : 0x%X", uvc_probe_control.bFrameIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwFrameInterval : %d", uvc_probe_control.dwFrameInterval, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wKeyFrameRate : 0x%X", uvc_probe_control.wKeyFrameRate, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wPFrameRate : 0x%X", uvc_probe_control.wPFrameRate, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wCompQuality : %d", uvc_probe_control.wCompQuality, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wCompWindowSize : 0x%X", uvc_probe_control.wCompWindowSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wDelay : 0x%X", uvc_probe_control.wDelay, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxVideoFrameSize : 0x%X", uvc_probe_control.dwMaxVideoFrameSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxPayloadTransferSize : 0x%X", uvc_probe_control.dwMaxPayloadTransferSize, 0, 0, 0, 0);
    }
}

static VOID uvcd_config_still_probe_control(UCHAR *data_pointer, ULONG request_length)
{
    uvcd_still_probe_control_t c;

    (VOID)request_length;

    if (data_pointer != NULL) {
        uvcd_decode_still_probe_control(data_pointer, &c);

        _ux_utility_print("UVC_Still_Probe_Control from Host: ");
        _ux_utility_print_uint5("\t bFormatIndex : 0x%X", c.bFormatIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFrameIndex : 0x%X", c.bFrameIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bCompressionIndex : 0x%X", c.bCompressionIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxVideoFrameSize : 0x%X", c.dwMaxVideoFrameSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxPayloadTransferSize : 0x%X", c.dwMaxPayloadTransferSize, 0, 0, 0, 0);

        if (c.bFormatIndex != 0U) {
            uvc_still_probe_control.bFormatIndex = c.bFormatIndex;
        }

        if (c.bFrameIndex != 0U) {
            uvc_still_probe_control.bFrameIndex = c.bFrameIndex;
        }

        if (c.bCompressionIndex != 0U) {
            uvc_still_probe_control.bCompressionIndex = c.bCompressionIndex;
        }

        if (c.dwMaxVideoFrameSize != 0U) {
            uvc_still_probe_control.dwMaxVideoFrameSize = c.dwMaxVideoFrameSize;
        }

        /* dwMaxPayloadTransferSize is only set by device. */
        if ((_ux_system_slave->ux_system_slave_speed == (ULONG)UX_SUPER_SPEED_DEVICE)) {
            uvc_still_probe_control.dwMaxPayloadTransferSize = 8 * 1024;
        } else if (_ux_system_slave->ux_system_slave_speed == (ULONG)UX_HIGH_SPEED_DEVICE) {
            uvc_still_probe_control.dwMaxPayloadTransferSize = 8 * 1024;
        } else {
            uvc_still_probe_control.dwMaxPayloadTransferSize = 8 * 200;
        }

        _ux_utility_print("UVC_Still_Probe_Control current : ");
        _ux_utility_print_uint5("\t bFormatIndex : 0x%X", uvc_still_probe_control.bFormatIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFrameIndex : 0x%X", uvc_still_probe_control.bFrameIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bCompressionIndex : 0x%X", uvc_still_probe_control.bCompressionIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxVideoFrameSize : 0x%X", uvc_still_probe_control.dwMaxVideoFrameSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxPayloadTransferSize : 0x%X", uvc_still_probe_control.dwMaxPayloadTransferSize, 0, 0, 0, 0);
    }
}


UINT _ux_device_class_uvc_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 *data = transfer_request->ux_slave_transfer_request_data_pointer;
    UINT uRet   = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Prob Control: SET_CUR");
        uvcd_config_probe_control(data, sreq->wLength);
        break;
    case UVC_GET_CUR:
        dbg("Prob Control: GET_CUR");
        uvcd_copy_probe_control(&uvc_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MIN:
        dbg("Prob Control: GET_MIN");
        uvcd_copy_probe_control(&uvc_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MAX:
        dbg("Prob Control: GET_MAX");
        uvcd_copy_probe_control(&uvc_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_DEF:
        dbg("Prob Control: GET_DEF");
        uvcd_copy_probe_control(&uvc_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_RES:
    case UVC_GET_LEN:
    case UVC_GET_INFO:
    default:
        _ux_utility_print_uint5("process_probe_control() : Unknown VS request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        uvcd_stall_controll_endpoint();
        uRet = UX_TRANSFER_STALLED;
        break;
    }
    return uRet;
}

UINT _ux_device_class_uvc_still_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 *data = transfer_request->ux_slave_transfer_request_data_pointer;
    UINT uRet   = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Still Prob Control: SET_CUR");
        uvcd_config_still_probe_control(data, sreq->wLength);
        break;
    case UVC_GET_CUR:
        dbg("Still Prob Control: GET_CUR");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MIN:
        dbg("Still Prob Control: GET_MIN");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MAX:
        dbg("Still Prob Control: GET_MAX");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_DEF:
        dbg("Still Prob Control: GET_DEF");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_RES:
    case UVC_GET_LEN:
    case UVC_GET_INFO:
    default:
        _ux_utility_print_uint5("process_still_probe_control() : Unknown VS request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        uvcd_stall_controll_endpoint();
        uRet = UX_TRANSFER_STALLED;
        break;
    }
    return uRet;
}

UINT32 uvcd_register_callback(const uvcd_event_callback_t *cbs)
{
    event_callbacks.encode_start = cbs->encode_start;
    event_callbacks.encode_stop  = cbs->encode_stop;
    event_callbacks.it_set       = cbs->it_set;
    event_callbacks.pu_set       = cbs->pu_set;
    event_callbacks.xu_handler   = cbs->xu_handler;
    return UX_SUCCESS;
}

void  uvcd_notify_encode_start(UINT32 idx)
{
    UINT32 uRet = 0;

    if (event_callbacks.encode_start != NULL) {
        uRet = event_callbacks.encode_start(idx);
    }
    if (uRet != 0U) {
        // ignore error
    }
}
void  uvcd_notify_encode_stop(UINT32 idx)
{
    UINT32 uRet = 0;

    if (event_callbacks.encode_stop != NULL) {
        uRet = event_callbacks.encode_stop(idx);
    }
    if (uRet != 0U) {
        // ignore error
    }
}

void  uvcd_notify_pu_set(UINT32 idx)
{
    if (event_callbacks.pu_set != NULL) {
        if (event_callbacks.pu_set(idx) != 0U) {
            // ignore error
        }
    }
}

void  uvcd_notify_it_set(UINT32 idx)
{
    if (event_callbacks.it_set != NULL) {
        if (event_callbacks.it_set(idx) != 0U) {
            // ignore error
        }
    }
}

UINT32 uvcd_notify_xu_handler(UINT32 code, UINT8 *buf, UINT32 buf_size, UINT32 request_length)
{
    UINT32 uRet;

    if (event_callbacks.xu_handler != NULL) {
        uRet = event_callbacks.xu_handler(code, buf, buf_size, request_length);
    } else {
        uRet = UX_ERROR;
    }
    return uRet;
}

