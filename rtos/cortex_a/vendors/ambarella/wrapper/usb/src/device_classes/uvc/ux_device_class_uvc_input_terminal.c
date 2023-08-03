/**
 *  @file ux_device_class_uvc_input_terminal.c
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
 *  @details control request functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"


#define UVC_SUPPORT_IT_CONTROLS (UVC_CT_CONTROL_AUTO_EXPOSURE_MODE_MASK | \
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

static UINT32 supported_it_selector = UVC_SUPPORT_IT_CONTROLS;
static UINT32 input_terminal_id     = 0x01;

static void dbg(const char *str)
{
	_ux_utility_print_uint5(str, 0, 0, 0, 0, 0);
}

UINT32 uvcd_set_input_terminal_id(UINT32 id)
{
    input_terminal_id = id;
    _ux_utility_print_uint5("input_terminal_id = %u", input_terminal_id, 0, 0, 0, 0);
    return UX_SUCCESS;
}

UINT32 uvcd_get_input_terminal_id(void)
{
    return input_terminal_id;
}

UINT32 uvcd_set_it_attribute(UINT32 attribute)
{
    supported_it_selector = attribute;
    return UX_SUCCESS;
}

UINT32 uvcd_get_it_attribute(void)
{
    return supported_it_selector;
}

static uvcd_it_auto_exposure_mode_t auto_exposure_mode_setting =
{
    UVC_AE_MODE_MANUAL,
    UVC_AE_MODE_MANUAL,
    UVC_AE_MODE_MANUAL,
    UVC_AE_MODE_MANUAL
};

static UINT uvcd_it_process_auto_exposure_mode_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Auto Exposure Mode Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = data[0];
        if ((value == UVC_AE_MODE_MANUAL) ||
            (value == UVC_AE_MODE_AUTO) ||
            (value == UVC_AE_MODE_SHUTTER_PRIORITY) ||
            (value == UVC_AE_MODE_APERTURE_PRIORITY)) {
            // Check if expose time absolute control is supported
            if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_EXPOSURE_TIME_ABSOLUTE_MASK) != 0U) {
                // send status interrupt if capability is changed.
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };
                UINT need_status_interrupt           = 0;

                uvc_status.uvcd_status_type = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator  = (UINT8)input_terminal_id;
                uvc_status.uvcd_event       = 0;    // control change
                uvc_status.uvcd_selector    = UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
                uvc_status.uvcd_attribute   = 0x01; // control info change
                if ((value == UVC_AE_MODE_AUTO) ||
                    (value == UVC_AE_MODE_APERTURE_PRIORITY)) {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL;
                    // need status interrupt?
                    if ((auto_exposure_mode_setting.current_value != UVC_AE_MODE_AUTO) &&
                        (auto_exposure_mode_setting.current_value != UVC_AE_MODE_APERTURE_PRIORITY)) {
                        need_status_interrupt = 1;
                    }
                } else {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT;
                    // need status interrupt?
                    if ((auto_exposure_mode_setting.current_value != UVC_AE_MODE_MANUAL) &&
                        (auto_exposure_mode_setting.current_value != UVC_AE_MODE_SHUTTER_PRIORITY)) {
                        need_status_interrupt = 1;
                    }
                }
                uvc_status.uvcd_value_length = 1;
                if (need_status_interrupt != 0U) {
                    if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                        // ignore this error
                    }
                }
            }


            // Check if iris absolute control is supported
            if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_IRIS_ABSOLUTE_MASK) != 0U) {
                // send status interrupt if capability is changed.
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };
                UINT need_status_interrupt           = 0;

                uvc_status.uvcd_status_type = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator  = (UINT8)input_terminal_id;
                uvc_status.uvcd_event       = 0;    // control change
                uvc_status.uvcd_selector    = UVC_CT_IRIS_ABSOLUTE_CONTROL;
                uvc_status.uvcd_attribute   = 0x01; // control info change
                if ((value == UVC_AE_MODE_AUTO) ||
                    (value == UVC_AE_MODE_SHUTTER_PRIORITY)) {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE;
                    // need status interrupt?
                    if ((auto_exposure_mode_setting.current_value != UVC_AE_MODE_AUTO) &&
                        (auto_exposure_mode_setting.current_value != UVC_AE_MODE_SHUTTER_PRIORITY)) {
                        need_status_interrupt = 1;
                    }
                } else {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT;
                    // need status interrupt?
                    if ((auto_exposure_mode_setting.current_value != UVC_AE_MODE_MANUAL) &&
                        (auto_exposure_mode_setting.current_value != UVC_AE_MODE_APERTURE_PRIORITY)) {
                        need_status_interrupt = 1;
                    }
                }
                uvc_status.uvcd_value_length = 1;

                if (need_status_interrupt != 0U) {
                    if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                        // ignore this error
                    }
                }
            }

            auto_exposure_mode_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_AE_MODE_CONTROL);
            uRet = UX_SUCCESS;
        } else {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        }
        break;
    case UVC_GET_CUR:
        dbg("Auto Exposure Mode Control : UVC_GET_CUR.");
        data[0] = auto_exposure_mode_setting.current_value;
        break;

    case UVC_GET_RES:
        dbg("Auto Exposure Mode Control : GET_RES.");
        _ux_utility_short_put(data, 0x0F);
        break;

    case UVC_GET_INFO:
        dbg("Auto Exposure Mode Control : GET_INFO.");
        _ux_utility_long_put(data, UVC_GET_SUPPORT | UVC_SET_SUPPORT);
        break;

    case UVC_GET_DEF:
        dbg("Auto Exposure Mode Control : GET_DEF.");
        data[0] = auto_exposure_mode_setting.default_value;
        break;

    default:
        _ux_utility_print_uint5("uvcd_it_process_auto_exposure_mode_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

static UINT uvcd_it_process_exposure_time_absolute_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_exposure_time_absoulte_t exposure_time_absolute_setting =
    {
        625,
        1,
        0x100,
        0x100
    };

    UINT32 value          = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Exposure Time Absolute Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = _ux_utility_long_get(data);

        if ((auto_exposure_mode_setting.current_value == UVC_AE_MODE_AUTO) ||
            (auto_exposure_mode_setting.current_value == UVC_AE_MODE_APERTURE_PRIORITY)) {
            _ux_utility_print("uvcd_it_process_exposure_time_absolute_control(): INVALID_CONTROL in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_CONTROL);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else if ((value > exposure_time_absolute_setting.maximum_value) || (value < exposure_time_absolute_setting.minimum_value)) {
            _ux_utility_print("uvcd_it_process_exposure_time_absolute_control(): OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            exposure_time_absolute_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Exposure Time Absolute Control : UVC_GET_CUR.");
        _ux_utility_long_put(data, exposure_time_absolute_setting.current_value);
        break;

    case UVC_GET_MIN:
        dbg("Exposure Time Absolute Control : GET_MIN.");
        _ux_utility_long_put(data, exposure_time_absolute_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Exposure Time Absolute Control : GET_MAX.");
        _ux_utility_long_put(data, exposure_time_absolute_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Exposure Time Absolute Control : GET_RES.");
        _ux_utility_long_put(data, 1);
        break;

    case UVC_GET_INFO:
        dbg("Exposure Time Absolute Control : GET_INFO.");
        if ((auto_exposure_mode_setting.current_value == UVC_AE_MODE_AUTO) ||
            (auto_exposure_mode_setting.current_value == UVC_AE_MODE_APERTURE_PRIORITY)) {
            _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL));
        } else {
            _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        }
        break;

    case UVC_GET_DEF:
        dbg("Exposure Time Absolute Control : GET_DEF.");
        _ux_utility_long_put(data, exposure_time_absolute_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("Exposure Time Absolute Control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

static uvcd_it_focus_auto_t focus_auto_setting =
{
    1,
    0,
    0,
    0
};

static UINT uvcd_it_process_focus_auto_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Focus auto control : UVC_SET_CUR.");
        is_tx_required                   = 0;
        value                            = data[0];
        focus_auto_setting.current_value = value;
        uvcd_notify_it_set(UVC_CT_FOCUS_AUTO_CONTROL);
        break;

    case UVC_GET_CUR:
        dbg("Focus auto control : UVC_GET_CUR.");
        data[0] = focus_auto_setting.current_value;
        break;

    case UVC_GET_INFO:
        dbg("Focus auto control : GET_INFO.");
        _ux_utility_long_put(data, UVC_GET_SUPPORT | UVC_SET_SUPPORT);
        break;

    case UVC_GET_DEF:
        dbg("Focus auto control : GET_DEF.");
        data[0] = focus_auto_setting.default_value;
        break;

    default:
        _ux_utility_print_uint5("Focus auto control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

static UINT uvcd_it_process_focus_absolute_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_focus_absolute_t focus_absolute_setting =
    {
        500,
        100,
        100,
        100
    };

    INT16 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Focus absolute control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (INT16)_ux_utility_short_get(data);
        if (focus_auto_setting.current_value == 1U) {
            _ux_utility_print("Focus absolute control : INVALID_CONTROL in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_CONTROL);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else if ((value > focus_absolute_setting.maximum_value) || (value < focus_absolute_setting.minimum_value)) {
            _ux_utility_print("Focus absolute control : OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            _ux_utility_print_uint5("----    Focus distance = %d mm.", (UINT)value, 0, 0, 0, 0);
            focus_absolute_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_FOCUS_ABSOLUTE_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Focus absolute control : UVC_GET_CUR.");
        _ux_utility_short_put(data, (USHORT)focus_absolute_setting.current_value);
        break;

    case UVC_GET_MIN:
        dbg("Focus absolute control : GET_MIN.");
        _ux_utility_short_put(data, (USHORT)focus_absolute_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Focus absolute control : GET_MAX.");
        _ux_utility_short_put(data, (USHORT)focus_absolute_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Focus absolute control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("Focus absolute control : GET_INFO.");
        if (focus_auto_setting.current_value == 1U) {
            _ux_utility_long_put(data, UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE);
        } else {
            _ux_utility_long_put(data, UVC_GET_SUPPORT | UVC_SET_SUPPORT);
        }
        break;

    case UVC_GET_DEF:
        dbg("Focus absolute control : GET_DEF.");
        _ux_utility_short_put(data, (USHORT)focus_absolute_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("Focus absolute control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

static UINT uvcd_it_process_zoom_absolute_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_zoom_absolute_t zoom_absolute_setting =
    {
        5,
        1,
        1,
        1
    };

    INT16 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Zoom absolute control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (INT16)_ux_utility_short_get(data);
        if ((value > zoom_absolute_setting.maximum_value) || (value < zoom_absolute_setting.minimum_value)) {
            _ux_utility_print("Zoom absolute control : OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            zoom_absolute_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_ZOOM_ABSOLUTE_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Zoom absolute control : UVC_GET_CUR.");
        _ux_utility_short_put(data, (USHORT)zoom_absolute_setting.current_value);
        break;

    case UVC_GET_MIN:
        dbg("Zoom absolute control : GET_MIN.");
        _ux_utility_short_put(data, (USHORT)zoom_absolute_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Zoom absolute control : GET_MAX.");
        _ux_utility_short_put(data, (USHORT)zoom_absolute_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Zoom absolute control : GET_RES.");
        _ux_utility_short_put(data, 1);
        break;

    case UVC_GET_INFO:
        dbg("Zoom absolute control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("Zoom absolute control : GET_DEF.");
        _ux_utility_short_put(data, (USHORT)zoom_absolute_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("Zoom absolute control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

static UINT uvcd_it_process_iris_absolute_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_iris_absolute_t iris_absolute_setting =
    {
        500,
        100,
        110,
        110
    };

    INT16 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Iris absolute control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (INT16)_ux_utility_short_get(data);
        if ((auto_exposure_mode_setting.current_value == UVC_AE_MODE_AUTO) ||
            (auto_exposure_mode_setting.current_value == UVC_AE_MODE_SHUTTER_PRIORITY)) {
            _ux_utility_print("Iris absolute control : INVALID_CONTROL in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_CONTROL);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else if ((value > iris_absolute_setting.maximum_value) || (value < iris_absolute_setting.minimum_value)) {
            _ux_utility_print("Iris absolute control: OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            iris_absolute_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_IRIS_ABSOLUTE_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Iris absolute control : UVC_GET_CUR.");
        _ux_utility_short_put(data, (USHORT)iris_absolute_setting.current_value);
        break;

    case UVC_GET_MIN:
        dbg("Iris absolute control : GET_MIN.");
        _ux_utility_short_put(data, (USHORT)iris_absolute_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Iris absolute control : GET_MAX.");
        _ux_utility_short_put(data, (USHORT)iris_absolute_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Iris absolute control : GET_RES.");
        _ux_utility_short_put(data, 1);
        break;

    case UVC_GET_INFO:
        dbg("Iris absolute control : GET_INFO.");
        dbg("Exposure Time Absolute Control : GET_INFO.");
        if ((auto_exposure_mode_setting.current_value == UVC_AE_MODE_AUTO) ||
            (auto_exposure_mode_setting.current_value == UVC_AE_MODE_SHUTTER_PRIORITY)) {
            _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE));
        } else {
            _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        }
        break;

    case UVC_GET_DEF:
        dbg("Iris absolute control : GET_DEF.");
        _ux_utility_short_put(data, (USHORT)iris_absolute_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("Iris absolute control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

#define UVC_SCANNING_MODE_CONTROL_INTERLACED  0U
#define UVC_SCANNING_MODE_CONTROL_PROGRESSIVE 1U

static UINT uvcd_it_process_scanning_mode_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_scanning_mode_t scanning_mode_setting =
    {
        UVC_SCANNING_MODE_CONTROL_PROGRESSIVE,
        UVC_SCANNING_MODE_CONTROL_INTERLACED,
        UVC_SCANNING_MODE_CONTROL_INTERLACED,
        UVC_SCANNING_MODE_CONTROL_INTERLACED
    };

    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Scanning mode control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = data[0];
        if ((value > scanning_mode_setting.maximum_value) || (value < scanning_mode_setting.minimum_value)) {
            _ux_utility_print("Scanning mode control : OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            scanning_mode_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_SCANNING_MODE_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Scanning mode control : UVC_GET_CUR.");
        data[0] = scanning_mode_setting.current_value;
        break;

    case UVC_GET_INFO:
        dbg("Scanning mode control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    default:
        _ux_utility_print_uint5("Scanning mode control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

#define UVC_AE_PRIORITY_FRAME_RATE_CONSTANT 0U
#define UVC_AE_PRIORITY_FRAME_RATE_DYNAMIC  1U

static UINT uvcd_it_process_auto_exposure_priority_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_auto_exposure_priority_t auto_exposure_priority_setting =
    {
        UVC_AE_PRIORITY_FRAME_RATE_DYNAMIC,
        UVC_AE_PRIORITY_FRAME_RATE_CONSTANT,
        UVC_AE_PRIORITY_FRAME_RATE_CONSTANT,
        UVC_AE_PRIORITY_FRAME_RATE_CONSTANT
    };

    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("AE priority control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = data[0];
        if ((value > auto_exposure_priority_setting.maximum_value) ||
            (value < auto_exposure_priority_setting.minimum_value)) {
            _ux_utility_print("AE priority control : OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            auto_exposure_priority_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_AE_PRIORITY_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("AE priority control : UVC_GET_CUR.");
        data[0] = auto_exposure_priority_setting.current_value;
        break;

    case UVC_GET_INFO:
        dbg("AE priority control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    default:
        _ux_utility_print_uint5("AE priority control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

static UINT uvcd_it_process_roll_absolute_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_roll_absolute_t roll_absolute_setting =
    {
        90,
        0,
        0,
        0
    };

    INT16 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Roll absolute control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (INT16)_ux_utility_short_get(data);
        if ((value > roll_absolute_setting.maximum_value) || (value < roll_absolute_setting.minimum_value)) {
            _ux_utility_print("Roll absolute control : OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            roll_absolute_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_ROLL_ABSOLUTE_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;

    case UVC_GET_CUR:
        dbg("Roll absolute control : UVC_GET_CUR.");
        _ux_utility_short_put(data, (USHORT)roll_absolute_setting.current_value);
        break;

    case UVC_GET_MIN:
        dbg("Roll absolute control : GET_MIN.");
        _ux_utility_short_put(data, (USHORT)roll_absolute_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Roll absolute control : GET_MAX.");
        _ux_utility_short_put(data, (USHORT)roll_absolute_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Roll absolute control : GET_RES.");
        _ux_utility_short_put(data, 1);
        break;

    case UVC_GET_INFO:
        dbg("Roll absolute control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("Roll absolute control : GET_DEF.");
        _ux_utility_short_put(data, (USHORT)roll_absolute_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("Roll absolute control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}



static uvcd_it_pantilt_absolute_t pantilt_absolute_setting =
{
    36000,
    36000,
    0,
    0,
    0,
    0,
    0,
    0
};

static UINT32 uvcd_copy_pantilt_absolute_setting(const uvcd_it_pantilt_absolute_t *src, uvcd_it_pantilt_absolute_t *dst)
{
    UINT32 uRet = UX_SUCCESS;

    if ((src == NULL) || (dst == NULL)) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        // avoid using memcpy to pass misra-c
        dst->cur_pan  = src->cur_pan;
        dst->cur_tilt = src->cur_tilt;
        dst->def_pan  = src->def_pan;
        dst->def_tilt = src->def_tilt;
        dst->max_pan  = src->max_pan;
        dst->max_tilt = src->max_tilt;
        dst->min_pan  = src->min_pan;
        dst->min_tilt = src->min_tilt;
    }
    return uRet;
}

static UINT uvcd_it_process_pantilt_absolute_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    INT32 pan = 0, tilt = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("PanTilt absolute control : UVC_SET_CUR.");
        is_tx_required = 0;
        pan            = (INT32)_ux_utility_long_get(data);
        tilt           = (INT32)_ux_utility_long_get(&data[4]);
        if ((pan > pantilt_absolute_setting.max_pan) ||
            (pan < pantilt_absolute_setting.min_pan) ||
            (tilt > pantilt_absolute_setting.max_tilt) ||
            (tilt < pantilt_absolute_setting.min_tilt)) {
            _ux_utility_print("PanTilt absolute control : OUT_OF_RANGE in UVC_SET_CUR");
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            pantilt_absolute_setting.cur_pan  = pan;
            pantilt_absolute_setting.cur_tilt = tilt;
            uvcd_notify_it_set(UVC_CT_PANTILT_ABSOLUTE_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;

    case UVC_GET_CUR:
        dbg("PanTilt absolute control : UVC_GET_CUR.");
        _ux_utility_long_put(data, (ULONG)pantilt_absolute_setting.cur_pan);
        _ux_utility_long_put(&data[4], (ULONG)pantilt_absolute_setting.cur_tilt);
        break;

    case UVC_GET_MIN:
        dbg("PanTilt absolute control : GET_MIN.");
        _ux_utility_long_put(data, (ULONG)pantilt_absolute_setting.min_pan);
        _ux_utility_long_put(&data[4], (ULONG)pantilt_absolute_setting.min_tilt);
        break;

    case UVC_GET_MAX:
        dbg("PanTilt absolute control : GET_MAX.");
        _ux_utility_long_put(data, (ULONG)pantilt_absolute_setting.max_pan);
        _ux_utility_long_put(&data[4], (ULONG)pantilt_absolute_setting.max_tilt);
        break;

    case UVC_GET_RES:
        dbg("PanTilt absolute control : GET_RES.");
        _ux_utility_long_put(data, 3600);
        _ux_utility_long_put(&data[4], 3600);
        break;

    case UVC_GET_INFO:
        dbg("PanTilt absolute control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("PanTilt absolute control : GET_DEF.");
        _ux_utility_long_put(data, (ULONG)pantilt_absolute_setting.def_pan);
        _ux_utility_long_put(&data[4], (ULONG)pantilt_absolute_setting.def_tilt);
        break;

    default:
        _ux_utility_print_uint5("PanTilt absolute control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

static UINT uvcd_it_process_privacy_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    static uvcd_it_privacy_t privacy_setting =
    {
        1,
        0,
        0,
        0
    };

    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Privacy control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = data[0];
        if ((value > privacy_setting.maximum_value) ||
            (value < privacy_setting.minimum_value)) {
            _ux_utility_print("Privacy control : OUT_OF_RANGE in UVC_SET_CUR");

            {
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };

                uvc_status.uvcd_status_type  = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator   = (UINT8)input_terminal_id;
                uvc_status.uvcd_event        = 0; // control change
                uvc_status.uvcd_selector     = UVC_CT_PRIVACY_CONTROL;
                uvc_status.uvcd_attribute    = 0x02;
                uvc_status.uvcd_value        = UVC_VC_ER_CD_OUT_OF_RANGE;
                uvc_status.uvcd_value_length = 1;
                if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                    // ignore this error
                }
            }

            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            {
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };

                uvc_status.uvcd_status_type  = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator   = (UINT8)input_terminal_id;
                uvc_status.uvcd_event        = 0;    // control change
                uvc_status.uvcd_selector     = UVC_CT_PRIVACY_CONTROL;
                uvc_status.uvcd_attribute    = 0x00; // control value change
                uvc_status.uvcd_value        = value;
                uvc_status.uvcd_value_length = 1;
                if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                    // ignore this error
                }
            }

            privacy_setting.current_value = value;
            uvcd_notify_it_set(UVC_CT_PRIVACY_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Privacy control : UVC_GET_CUR.");
        data[0] = privacy_setting.current_value;
        break;

    case UVC_GET_INFO:
        dbg("Privacy control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL | UVC_ASYNCHRONOUS_CONTROL));
        break;

    default:
        _ux_utility_print_uint5("Privacy control : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
        uvcd_set_request_error_code(UVC_VC_ER_CD_INVALID_REQUEST);
        uvcd_stall_controll_endpoint();
        is_tx_required = 0;
        uRet           = UX_TRANSFER_STALLED;
        break;
    }
    if (is_tx_required == 1U) {
        uRet = _ux_device_stack_transfer_request(transfer, sreq->wLength, sreq->wLength);
    }
    return uRet;
}

UINT32 uvcd_get_it_pantile_abs(uvcd_it_pantilt_absolute_t *s)
{
    return uvcd_copy_pantilt_absolute_setting(&pantilt_absolute_setting, s);
}

UINT32 uvcd_set_it_pantile_abs(const uvcd_it_pantilt_absolute_t *s)
{
    return uvcd_copy_pantilt_absolute_setting(s, &pantilt_absolute_setting);
}

UINT uvcd_process_request_it(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 cs_id = (UINT8)((sreq->wValue & 0xFF00U) >> 8U);
    UINT uRet;

    switch (cs_id) {
    case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_EXPOSURE_TIME_ABSOLUTE_MASK) != 0U) {
            uRet = uvcd_it_process_exposure_time_absolute_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;
    case UVC_CT_AE_MODE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_AUTO_EXPOSURE_MODE_MASK) != 0U) {
            uRet = uvcd_it_process_auto_exposure_mode_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_FOCUS_ABSOLUTE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_FOCUS_ABSOLUTE_MASK) != 0U) {
            uRet = uvcd_it_process_focus_absolute_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_FOCUS_AUTO_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_FOCUS_AUTO_MASK) != 0U) {
            uRet = uvcd_it_process_focus_auto_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_ZOOM_ABSOLUTE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_ZOOM_ABSOLUTE_MASK) != 0U) {
            uRet = uvcd_it_process_zoom_absolute_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_IRIS_ABSOLUTE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_IRIS_ABSOLUTE_MASK) != 0U) {
            uRet = uvcd_it_process_iris_absolute_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_SCANNING_MODE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_SCANNING_MODE_MASK) != 0U) {
            uRet = uvcd_it_process_scanning_mode_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_AE_PRIORITY_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_AUDO_EXPOSURE_PRIORITY_MASK) != 0U) {
            uRet = uvcd_it_process_auto_exposure_priority_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_ROLL_ABSOLUTE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_ROLL_ABSOLUTE_MASK) != 0U) {
            uRet = uvcd_it_process_roll_absolute_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_PANTILT_ABSOLUTE_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_PAN_TILE_ABSOLUTE_MASK) != 0U) {
            uRet = uvcd_it_process_pantilt_absolute_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_CT_PRIVACY_CONTROL:
        if ((uvcd_get_it_attribute() & UVC_CT_CONTROL_PRIVACY_MASK) != 0U) {
            uRet = uvcd_it_process_privacy_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;
    case UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
    case UVC_CT_IRIS_RELATIVE_CONTROL:
    case UVC_CT_FOCUS_RELATIVE_CONTROL:
    case UVC_CT_PANTILT_RELATIVE_CONTROL:
    case UVC_CT_ROLL_RELATIVE_CONTROL:
    case UVC_CT_ZOOM_RELATIVE_CONTROL:
        _ux_utility_print_uint5("Unsupported IT Control Selector(0x%X)", cs_id, 0, 0, 0, 0);
        uRet = uvcd_process_unsupported_control();
        break;
    default:
        _ux_utility_print_uint5("Unknown IT Control Selector(0x%X)", cs_id, 0, 0, 0, 0);
        uRet = uvcd_process_unsupported_control();
        break;
    }

    return uRet;
}

