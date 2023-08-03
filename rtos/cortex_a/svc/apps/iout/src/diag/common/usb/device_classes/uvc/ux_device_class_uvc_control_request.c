/**
 *  @file ux_device_class_uvc_control_request.c
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
 *  @details control request functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

static UINT32 request_error_code = 0x0FF;  // 0xFF means unknown

static UINT process_request_error_code_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 *data = transfer->ux_slave_transfer_request_data_pointer;

    switch (sreq->bRequest) {
    case UVC_GET_CUR:
        _ux_utility_print_uint5("[UVC] Request Error Code Control : UVC_GET_CUR, code 0x%X", request_error_code, 0, 0, 0, 0);
        data[0] = (UINT8)(request_error_code & 0x0FFU);
        break;

    case UVC_GET_INFO:
        _ux_utility_print("[UVC] Request Error Code Control : GET_INFO.");
        data[0] = UVC_GET_SUPPORT;
        break;

    default:
        _ux_utility_print_uint5("[UVC] Request Error Code Control : Unknown VC request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        break;
    }

    return _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
}

static UINT process_power_mode_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static UINT8 uvc_pwoer_mode = 0x80U;

    UINT8 *data          = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 uret          = 0;
    UINT8 is_tx_required = 1;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        is_tx_required = 0;
        _ux_utility_print("Power Mode Control : UVC_SET_CUR.");
        uret = UX_SUCCESS;
        break;

    case UVC_GET_CUR:
        _ux_utility_print("Power Mode Control : UVC_GET_CUR.");
        data[0] = uvc_pwoer_mode;
        break;

    case UVC_GET_INFO:
        _ux_utility_print("Power Mode Control : GET_INFO.");
        data[0] = (UINT8)(UVC_GET_SUPPORT | UVC_SET_SUPPORT);
        break;
    default:
        _ux_utility_print_uint5("Power Mode Control : Unknown VC request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        break;
    }
    if (is_tx_required == 1U) {
        uret = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uret;
}

static UINT process_still_image_trigger_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static UINT8 btrigger = 0;
    UINT8  *data = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 if_id = ((UINT32)sreq->wIndex) & 0x00FFUL;
    UINT8  is_tx_required = 1;
    UINT32 uret           = 0;

    switch(sreq->bRequest) {
    case UVC_SET_CUR:
        _ux_utility_print_uint5("Still Image Trigger: UVC_SET_CUR to %d.", data[0], 0, 0, 0, 0);
        btrigger = data[0];
        uxd_uvc_stillcap_start_notify(if_id - 1UL);
        is_tx_required = 0;
        uret = UX_SUCCESS;
        break;
    case UVC_GET_CUR:
        _ux_utility_print("Still Image Trigger: UVC_GET_CUR.");
        data[0] = btrigger;
        break;

    case UVC_GET_INFO:
        _ux_utility_print("Still Image Trigger: GET_INFO.");
        data[0] = (UINT8)(UVC_GET_SUPPORT | UVC_SET_SUPPORT);
        break;
    default:
        _ux_utility_print_uint5("Still Image Trigger: Unknown VC request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        break;
    }
    if (is_tx_required == 1U) {
        uret = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uret;
}

static UINT process_video_control_request(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 cs_id = (sreq->wValue >> 8U) & 0x0FFU;
    UINT32 uret  = UX_FUNCTION_NOT_SUPPORTED;

    switch (cs_id) {
    case UVC_VC_VIDEO_POWER_MODE_CONTROL:
        uret = process_power_mode_control(transfer, sreq);
        break;
    case UVC_VC_REQUEST_ERROR_CODE_CONTROL:
        uret = process_request_error_code_control(transfer, sreq);
        break;
    default:
        _ux_utility_print_uint5("process_video_control_request() : Unknown VS Control Selector(0x%X)", cs_id, 0, 0, 0, 0);
        break;
    }
    return uret;
}

static UINT process_video_streaming_request(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 cs_id = (sreq->wValue >> 8U) & 0x00FFU;
    UINT32 uret  = UX_FUNCTION_NOT_SUPPORTED;

    switch (cs_id) {
    case UVC_VS_PROBE_CONTROL:
        uret = uxd_uvc_probe_control(transfer, sreq);
        break;
    case UVC_VS_COMMIT_CONTROL:
        uret = uxd_uvc_commit_control(transfer, sreq);
        break;
    case UVC_VS_STILL_PROBE_CONTROL:
        uret = uxd_uvc_still_probe_control(transfer, sreq);
        break;
    case UVC_VS_STILL_COMMIT_CONTROL:
        uret = uxd_uvc_still_commit_control(transfer, sreq);
        break;
    case UVC_VS_STILL_IMAGE_TRIGGER_CONTROL:
        uret = process_still_image_trigger_control(transfer, sreq);
        break;
    case UVC_VS_GENERATE_KEY_FRAME_CONTROL:
    case UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL:
    case UVC_VS_STREAM_ERROR_CODE_CONTROL:
    default:
        _ux_utility_print_uint5("process_video_streaming_request() : Unknown VS Control Selector(0x%X)", cs_id, 0, 0, 0, 0);
        break;
    }
    return uret;
}

static UINT uvcd_process_request_oc(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 if_id = (sreq->wIndex & 0x00FFU);
    UINT uret;

    switch (if_id) {
    case 0:
        uret = process_video_control_request(transfer, sreq);
        break;
    default:
        uret = process_video_streaming_request(transfer, sreq);
        break;
    }
    return uret;
}
/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * Set error code for last UVC Control request. It will be retrieve by another UVC control request.
*/
void uxd_uvc_request_error_code_set(UINT32 value)
{
    request_error_code = value;
}
/**
 * Stall control endpoint
*/
void uxd_uvc_controll_endpoint_stall(void)
{
    UX_SLAVE_DEVICE *device = &_ux_system_slave->ux_system_slave_device;

    if (_ux_device_stack_endpoint_stall(&device->ux_slave_device_control_endpoint) != 0U) {
        // ignore this error
    }
}
/**
 * Send NULL packet over control endpoint
*/
void uxd_uvc_ctrl_null_packet_send(UX_SLAVE_TRANSFER *transfer)
{
    if (_ux_device_stack_transfer_request(transfer, 0, 0) != 0U) {
        // ignore this error
    }
}

/**
 * Called by USBX to handle USB Control requests
*/
UINT  uxd_uvc_control_request(UX_SLAVE_CLASS_COMMAND *command)
{
    UX_SLAVE_DEVICE *ux_device;
    UX_SLAVE_TRANSFER *transfer;
    USB_SETUP_REQUEST_S setup_request;
    UINT32 uret = UX_SUCCESS;

    (VOID)command;

    ux_device = &_ux_system_slave->ux_system_slave_device;

    transfer = &ux_device->ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;
    setup_request.bmRequestType = transfer->ux_slave_transfer_request_setup[0];
    setup_request.bRequest      = transfer->ux_slave_transfer_request_setup[1];
    setup_request.wValue        = (UINT16)_ux_utility_short_get(&transfer->ux_slave_transfer_request_setup[2]);
    setup_request.wIndex        = (UINT16)_ux_utility_short_get(&transfer->ux_slave_transfer_request_setup[4]);
    setup_request.wLength       = (UINT16)_ux_utility_short_get(&transfer->ux_slave_transfer_request_setup[6]);

#ifdef DEBUG
    _ux_utility_print("Setup Request:");
    _ux_utility_print_uint5("    bmRequestType = 0x%X", setup_request->bmRequestType, 0, 0, 0, 0);
    _ux_utility_print_uint5("    bRequest = 0x%X", setup_request->bRequest, 0, 0, 0, 0);
    _ux_utility_print_uint5("    wIndex = 0x%X", setup_request->wIndex, 0, 0, 0, 0);
    _ux_utility_print_uint5("    wValue = 0x%X", setup_request->wValue, 0, 0, 0, 0);
    _ux_utility_print_uint5("    wLength = 0x%X", setup_request->wLength, 0, 0, 0, 0);
#endif

    if (((setup_request.bmRequestType & 0x80U) == 0U) && (setup_request.wLength != 0U)) {
#ifdef DEBUG
        _ux_utility_print_uint5("_ux_device_class_uvc_control_request(): Control-Out wLength %d.", setup_request.wLength, 0, 0, 0, 0);
#endif
        if (transfer->max_buffer_size < setup_request.wLength) {
            _ux_utility_print_uint5("_ux_device_class_uvc_control_request(): Control-Out wLength %d but max buffer size is %d.", setup_request.wLength, transfer->max_buffer_size, 0, 0, 0);
            uxd_uvc_controll_endpoint_stall();
            uret = UX_MEMORY_ARRAY_FULL;
        } else {
            // switch control transfer phase to RX for control-out.
            transfer->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_IN;
            uret                                      = _ux_device_stack_transfer_request(transfer, setup_request.wLength, setup_request.wLength);
            if (uret != 0U) {
                _ux_utility_print_uint5("_ux_device_class_uvc_control_request(): Can't get data for Class Request, code 0x%X.", uret, 0, 0, 0, 0);
            }
        }
    }

    if (uret == 0U) {
        // switch control transfer phase to TX for control-in.
        // NULL packet will be sent by Driver.
        transfer->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_OUT;
        uret = uxd_uvc_ctrl_request_notify(&setup_request,
                                           transfer->ux_slave_transfer_request_data_pointer,
                                           transfer->max_buffer_size);

        if (uret == 0U) {
            if ((setup_request.bmRequestType & 0x80U) != 0U) {
                // Device-to-Host request, must send data back to Host
                uret = _ux_device_stack_transfer_request(transfer, setup_request.wLength, setup_request.wLength);
            }
        } else if (uret == 0xFFFFFFFFU) {
            uret = uvcd_process_request_oc(transfer, &setup_request);
        } else {
            uxd_uvc_request_error_code_set(uret);
            uxd_uvc_controll_endpoint_stall();
            uret = UX_TRANSFER_STALLED;
        }
    }
    return uret;
}
/** @} */
