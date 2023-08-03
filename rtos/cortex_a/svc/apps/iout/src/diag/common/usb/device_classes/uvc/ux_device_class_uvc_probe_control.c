/**
 *  @file ux_device_class_uvc_probe_control.c
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
 *  @details probe control functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

static uvcd_host_config_t uvc_host_configs[UVC_MAX_STREAM_NUMBER]    = { 0 };
static uvcd_event_callback_t event_callbacks                         = { NULL, NULL, NULL, NULL };

static uvcd_probe_control_t uvc_probe_controls[UVC_MAX_STREAM_NUMBER] = {
    // stream 0
    {
        0x0000,                  /* bmHint,
                                    D0: dwFrameInterval
                                    D1: wKeyFrameRate
                                    D2: wPFrameRate
                                    D3: wCompQuality
                                    D4: wCompWindowSize      */
        0x01,                    /* bFormatIndex */
        0x01,                    /* bFrameIndex */
        0x00051615,              /* dwFrameInterval, 30 fps */
        0x0000,                  /* wKeyFrameRate */
        0x0000,                  /* wPFrameRate */
        5000,                    /* wCompQuality */
        0x0000,                  /* wCompWindowSize */
        0x0000,                  /* wDelay */
        0xE1000,                 /* dwMaxVideoFrameSize */
        512,                     /* dwMaxPayloadTransferSize */
    },
    // stream 1
    {
        0x0000,                  /* bmHint,
                                    D0: dwFrameInterval
                                    D1: wKeyFrameRate
                                    D2: wPFrameRate
                                    D3: wCompQuality
                                    D4: wCompWindowSize      */
        0x01,                    /* bFormatIndex */
        0x01,                    /* bFrameIndex */
        0x00051615,              /* dwFrameInterval, 30 fps */
        0x0000,                  /* wKeyFrameRate */
        0x0000,                  /* wPFrameRate */
        5000,                    /* wCompQuality */
        0x0000,                  /* wCompWindowSize */
        0x0000,                  /* wDelay */
        0xE1000,                 /* dwMaxVideoFrameSize */
        512,                     /* dwMaxPayloadTransferSize */
    },
    // stream 2
    {
        0x0000,                  /* bmHint,
                                    D0: dwFrameInterval
                                    D1: wKeyFrameRate
                                    D2: wPFrameRate
                                    D3: wCompQuality
                                    D4: wCompWindowSize      */
        0x01,                    /* bFormatIndex */
        0x01,                    /* bFrameIndex */
        0x00051615,              /* dwFrameInterval, 30 fps */
        0x0000,                  /* wKeyFrameRate */
        0x0000,                  /* wPFrameRate */
        5000,                    /* wCompQuality */
        0x0000,                  /* wCompWindowSize */
        0x0000,                  /* wDelay */
        0xE1000,                 /* dwMaxVideoFrameSize */
        512,                     /* dwMaxPayloadTransferSize */
    }
};

static uvcd_still_probe_control_t uvc_still_probe_control = {
    1,           /* bFormatIndex */
    1,           /* bFrameIndex */
    0,           /* bCompressionIndex */
    1024*1024*4, /* dwMaxVideoFrameSize */
    1024*1024,   /* dwMaxPayloadTransferSize */
};

static void dbg(const char *str)
{
    _ux_utility_print_uint5(str, 0, 0, 0, 0, 0);
}


static void uvcd_copy_still_probe_control(const uvcd_still_probe_control_t *control, UINT8 *out)
{
    out[0] = control->bFormatIndex;
    out[1] = control->bFrameIndex;
    out[2] = control->bCompressionIndex;
    _ux_utility_long_put(&out[3], control->dwMaxVideoFrameSize);
    _ux_utility_long_put(&out[7], control->dwMaxPayloadTransferSize);
}

void uxd_uvc_still_probe_config(UCHAR *data_pointer, ULONG request_length)
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
            uvc_still_probe_control.dwMaxPayloadTransferSize = 1024 * 1024;
        } else if (_ux_system_slave->ux_system_slave_speed == (ULONG)UX_HIGH_SPEED_DEVICE) {
            uvc_still_probe_control.dwMaxPayloadTransferSize = 1024 * 1024;
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


/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * Get Current proble control setting
*/
uvcd_probe_control_t *uxd_uvc_probe_control_get(UINT32 stream_id)
{
    uvcd_probe_control_t *pc = &uvc_probe_controls[0];

    if (stream_id < UVC_MAX_STREAM_NUMBER) {
        pc = &uvc_probe_controls[stream_id];
    }
    return pc;
}

/**
 * Get Current still proble control setting
*/
uvcd_still_probe_control_t *uxd_uvc_still_probe_control_get(void)
{
    return &uvc_still_probe_control;
}

/**
 * Copy proble control setting into output buffer
*/
void uxd_uvc_probe_control_copy(const uvcd_probe_control_t *control, UINT8 *out)
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

/**
 * Decode proble control setting from input buffer
*/
void uxd_uvc_probe_control_decode(UINT8 *data, uvcd_probe_control_t *control)
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

/**
 * Decode still proble control setting from input buffer
*/
void uvcd_decode_still_probe_control(UINT8 *data, uvcd_still_probe_control_t *control)
{
    control->bFormatIndex             = data[0];
    control->bFrameIndex              = data[1];
    control->bCompressionIndex        = data[2];
    control->dwMaxVideoFrameSize      = _ux_utility_long_get(&data[3]);
    control->dwMaxPayloadTransferSize = _ux_utility_long_get(&data[7]);
}

/**
 * This function is used to get current configurations set by Host.
 * @param StreamID [Input] Stream ID for the configuration.
 * @param pConfig  [Output] Information on Host configurations.
 * @retval 0
 * @retval UX_PARAMETER_ERROR
*/
UINT uxd_uvc_host_config_get(UINT StreamID, uvcd_host_config_t *pConfig)
{
    UINT uret = 0;
    if (StreamID < UVC_MAX_STREAM_NUMBER) {
        _ux_utility_memory_copy(pConfig, &uvc_host_configs[StreamID], sizeof(uvcd_host_config_t));
    } else {
        uret = UX_PARAMETER_ERROR;
    }
    return uret;
}

/**
 * This function is used to get the maximum payload transfer size which is one of the probe controls.
 * @param StreamID [Input]  Stream ID to get the size
 * @param pSize    [Output] Maximum payload transfer size
 * @retval 0
 * @retval UX_PARAMETER_ERROR
*/
UINT uxd_uvc_probe_max_xfer_size_get(UINT StreamID, UINT *pSize)
{
    UINT uret = 0;
    if ((StreamID < UVC_MAX_STREAM_NUMBER) && (pSize != NULL)) {
        *pSize = uvc_probe_controls[StreamID].dwMaxPayloadTransferSize;
    } else {
        uret = UX_PARAMETER_ERROR;
    }
    return uret;
}

static UINT flag_max_payload_set[UVC_MAX_STREAM_NUMBER] = {0};

/**
 * This function is used to set maximum payload transfer size which is one of probe controls.
 * This value is set by the device and read only from the host.
 * If the value is not set by the application, the default value will be used.
 * Application can use AmbaUSBD_UVCProbeMaxXferSizeGet() to get the default value.
 * @param StreamID [Input]  Stream ID to set the size
 * @param Size Maximum payload transfer size
*/
void uxd_uvc_probe_max_xfer_size_set(UINT StreamID, UINT Size)
{
    if (StreamID < UVC_MAX_STREAM_NUMBER) {
        if (Size > 0U) {
            flag_max_payload_set[StreamID]                        = 1;
            uvc_probe_controls[StreamID].dwMaxPayloadTransferSize = Size;
        } else {
            flag_max_payload_set[StreamID]                        = 0;
            uvc_probe_controls[StreamID].dwMaxPayloadTransferSize = 0;
        }
    }
}

/**
 * Configure current probe control setting according to input buffer.
*/
VOID uxd_uvc_probe_control_config(UINT32 stream_id, UCHAR *data_pointer, ULONG request_length)
{
    uvcd_probe_control_t c;

    (VOID)request_length;

    if ((data_pointer != NULL) && (stream_id < UVC_MAX_STREAM_NUMBER)) {
        uxd_uvc_probe_control_decode(data_pointer, &c);

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
            uvc_probe_controls[stream_id].bFormatIndex = c.bFormatIndex;
            uvc_host_configs[stream_id].format_index   = c.bFormatIndex;
        }

        if (c.bFrameIndex != 0U) {
            uvc_probe_controls[stream_id].bFrameIndex = c.bFrameIndex;
            uvc_host_configs[stream_id].frame_index   = c.bFrameIndex;
        }

        /* config frame rate */
        if (c.dwFrameInterval != 0U) {
            if (uvc_probe_controls[stream_id].dwFrameInterval != c.dwFrameInterval) {
                uvc_probe_controls[stream_id].dwFrameInterval = c.dwFrameInterval;
            }
        }

        if (uvc_probe_controls[stream_id].wCompQuality != c.wCompQuality) {
            UINT32 mjpeg_quality;
            uvc_probe_controls[stream_id].wCompQuality = c.wCompQuality;
            mjpeg_quality                              = (UINT32)uvc_probe_controls[stream_id].wCompQuality / 100U;
            if (mjpeg_quality < 10U) {
                _ux_utility_print_uint5("[Warning] config_uvc_probe_control(): mjpeg quality(%d) < 10, set to 10.", mjpeg_quality, 0, 0, 0, 0);
                mjpeg_quality = 10;
            }
            if (mjpeg_quality > 90U) {
                _ux_utility_print_uint5("[Warning] config_uvc_probe_control(): mjpeg quality(%d) > 90, set to 90.", mjpeg_quality, 0, 0, 0, 0);
                mjpeg_quality = 90U;
            }
            uvc_host_configs[stream_id].mjpeg_quality = mjpeg_quality;
        }

        if (c.dwMaxVideoFrameSize != 0U) {
            uvc_probe_controls[stream_id].dwMaxVideoFrameSize = c.dwMaxVideoFrameSize;
        }

        /* dwMaxPayloadTransferSize is only set by device. */
        // The value is determined in uvc_probe_control.dwMaxPayloadTransferSize.
        // Application can change current value through probe control API before UVC starts.
        if (flag_max_payload_set[stream_id] == 0U) {
            if ((_ux_system_slave->ux_system_slave_speed == (ULONG)UX_SUPER_SPEED_DEVICE)) {
                // (Todo) set the dwMaxPayloadTransferSize to 1MB for Super Speed Bulk pipe.
                uvc_probe_controls[stream_id].dwMaxPayloadTransferSize = 1024 * 1024;
            } else if (_ux_system_slave->ux_system_slave_speed == (ULONG)UX_HIGH_SPEED_DEVICE) {
                // set the dwMaxPayloadTransferSize to 1MB for High Speed Bulk pipe.
                uvc_probe_controls[stream_id].dwMaxPayloadTransferSize = 1024 * 1024;
            } else {
                // set the dwMaxPayloadTransferSize to 200KB for Full Speed Bulk pipe
                // OHCI only has 256KB buffer, if the USB packet is larger than 256KB, the HOST(Windows) will
                // crash(BSOD).
                // UHCI doesn't have this limitation.
                uvc_probe_controls[stream_id].dwMaxPayloadTransferSize = 1024 * 200;
            }
        }

        uvc_host_configs[stream_id].framerate = 10000000U / uvc_probe_controls[stream_id].dwFrameInterval;

        _ux_utility_print("UVC_Probe_Control current : ");
        _ux_utility_print_uint5("\t bmHint : 0x%X", uvc_probe_controls[stream_id].bmHint, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFormatIndex : 0x%X", uvc_probe_controls[stream_id].bFormatIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t bFrameIndex : 0x%X", uvc_probe_controls[stream_id].bFrameIndex, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwFrameInterval : %d", uvc_probe_controls[stream_id].dwFrameInterval, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wKeyFrameRate : 0x%X", uvc_probe_controls[stream_id].wKeyFrameRate, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wPFrameRate : 0x%X", uvc_probe_controls[stream_id].wPFrameRate, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wCompQuality : %d", uvc_probe_controls[stream_id].wCompQuality, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wCompWindowSize : 0x%X", uvc_probe_controls[stream_id].wCompWindowSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t wDelay : 0x%X", uvc_probe_controls[stream_id].wDelay, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxVideoFrameSize : 0x%X", uvc_probe_controls[stream_id].dwMaxVideoFrameSize, 0, 0, 0, 0);
        _ux_utility_print_uint5("\t dwMaxPayloadTransferSize : 0x%X", uvc_probe_controls[stream_id].dwMaxPayloadTransferSize, 0, 0, 0, 0);
    }
}

/**
 * Entry point for handling proble control requests.
*/
UINT uxd_uvc_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 *data = transfer_request->ux_slave_transfer_request_data_pointer;
    UINT   uret   = UX_SUCCESS;
    UINT16 stream_id = (sreq->wIndex & 0x00FFU) - 1U;

    if (stream_id >= UVC_MAX_STREAM_NUMBER) {
        stream_id = 0;
    }

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Prob Control: SET_CUR");
        uxd_uvc_probe_control_config(stream_id, data, sreq->wLength);
        break;
    case UVC_GET_CUR:
        dbg("Prob Control: GET_CUR");
        uxd_uvc_probe_control_copy(&uvc_probe_controls[stream_id], data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MIN:
        dbg("Prob Control: GET_MIN");
        uxd_uvc_probe_control_copy(&uvc_probe_controls[stream_id], data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MAX:
        dbg("Prob Control: GET_MAX");
        uxd_uvc_probe_control_copy(&uvc_probe_controls[stream_id], data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_DEF:
        dbg("Prob Control: GET_DEF");
        uxd_uvc_probe_control_copy(&uvc_probe_controls[stream_id], data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_RES:
    case UVC_GET_LEN:
    case UVC_GET_INFO:
    default:
        _ux_utility_print_uint5("process_probe_control() : Unknown VS request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        uxd_uvc_controll_endpoint_stall();
        uret = UX_TRANSFER_STALLED;
        break;
    }
    return uret;
}

/**
 * Entry point for handling still proble control requests.
*/
UINT uxd_uvc_still_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 *data = transfer_request->ux_slave_transfer_request_data_pointer;
    UINT uret   = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Still Prob Control: SET_CUR");
        uxd_uvc_still_probe_config(data, sreq->wLength);
        break;
    case UVC_GET_CUR:
        dbg("Still Prob Control: GET_CUR");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MIN:
        dbg("Still Prob Control: GET_MIN");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_MAX:
        dbg("Still Prob Control: GET_MAX");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_DEF:
        dbg("Still Prob Control: GET_DEF");
        uvcd_copy_still_probe_control(&uvc_still_probe_control, data);
        uret = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    case UVC_GET_RES:
    case UVC_GET_LEN:
    case UVC_GET_INFO:
    default:
        _ux_utility_print_uint5("process_still_probe_control() : Unknown VS request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        uxd_uvc_controll_endpoint_stall();
        uret = UX_TRANSFER_STALLED;
        break;
    }
    return uret;
}

/**
 * This function is used to register callback functions for UVC events.
 * @param pCallBacks Information on callback functions.
*/
UINT uxd_uvc_callback_register(const uvcd_event_callback_t *pCallBacks)
{
    event_callbacks.encode_start        = pCallBacks->encode_start;
    event_callbacks.encode_stop         = pCallBacks->encode_stop;
    event_callbacks.still_capture_start = pCallBacks->still_capture_start;
    event_callbacks.request_handler     = pCallBacks->request_handler;
    return UX_SUCCESS;
}

/**
 * Notify appplication that Host wants to start video streaming
*/
void  uxd_uvc_encode_start_notify(UINT idx)
{
    UINT32 uret = 0;

    if (event_callbacks.encode_start != NULL) {
        uret = event_callbacks.encode_start(idx);
    }
    if (uret != 0U) {
        // ignore error
    }
}

/**
 * Notify appplication that Host wants to stop video streaming
*/
void  uxd_uvc_encode_stop_notify(UINT idx)
{
    UINT32 uret = 0;

    if (event_callbacks.encode_stop != NULL) {
        uret = event_callbacks.encode_stop(idx);
    }
    if (uret != 0U) {
        // ignore error
    }
}

void  uxd_uvc_stillcap_start_notify(UINT idx)
{
    // If upper layer registers multiple stream callback, use it.
    if (event_callbacks.still_capture_start != NULL) {
        if (event_callbacks.still_capture_start(idx) != 0U) {
            // action TBD
        }
    } else {
        _ux_utility_print("udc_uvc_notify_still_capture_start(): no callback registered.");
    }
}

/**
 * Notify appplication that Host sends a control request
*/
UINT32 uxd_uvc_ctrl_request_notify(const USB_SETUP_REQUEST_S *setup_request, UINT8 *buffer, UINT32 buffer_size)
{
    UINT32 uret;

    if (event_callbacks.request_handler != NULL) {
        uret = event_callbacks.request_handler(setup_request, buffer, buffer_size);
    } else {
        uret = UX_ERROR;
    }
    return uret;
}
/** @} */

