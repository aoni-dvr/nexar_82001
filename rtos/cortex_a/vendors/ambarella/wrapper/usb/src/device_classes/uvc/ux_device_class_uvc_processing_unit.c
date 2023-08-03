/**
 *  @file ux_device_class_uvc_processing_unit.c
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
 *  @details processing unit functions of the uvc device class
 */

#define UX_SOURCE_CODE

/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

//#define UVC_XU_CONTROL_MAX_LEN      10

#define UVC_SUPPORT_PU_CONTROLS (UVC_PU_CONTROL_BRIGHTNESS_MASK | \
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
                                 UVC_PU_CONTROL_DIGITAL_MULT_MASK | \
                                 UVC_PU_CONTROL_DIGITAL_MULT_LIMIT_MASK  \
                                 )

static UINT32 processing_unit_id = 0x05;

static void dbg(const char *str)
{
	_ux_utility_print_uint5(str, 0, 0, 0, 0, 0);
}

UINT32 uvcd_set_processing_unit_id(UINT32 id)
{
    processing_unit_id = id;
    _ux_utility_print_uint5("processing_unit_id = %d", processing_unit_id, 0, 0, 0, 0);
    return UX_SUCCESS;
}

UINT32 uvcd_get_processing_unit_id(void)
{
    return processing_unit_id;
}

static UINT32 supported_pu_selector = UVC_SUPPORT_PU_CONTROLS;

UINT32 uvcd_set_pu_attribute(UINT32 attribute)
{
    supported_pu_selector = attribute;
    return UX_SUCCESS;
}

UINT32 uvcd_get_pu_attribute(void)
{
    return supported_pu_selector;
}

static uvcd_pu_brightness_t brightness_setting =
{
    256,
    -256,
    0,
    0
};

UINT32 uvcd_pu_get_brightness(uvcd_pu_brightness_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = brightness_setting.current_value;
        get->default_value = brightness_setting.default_value;
        get->maximum_value = brightness_setting.maximum_value;
        get->minimum_value = brightness_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_brightness(const uvcd_pu_brightness_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        brightness_setting.current_value = set->current_value;
        brightness_setting.default_value = set->default_value;
        brightness_setting.maximum_value = set->maximum_value;
        brightness_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_brightness_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    INT16 brightness = 0;
    UINT8 *data      = transfer->ux_slave_transfer_request_data_pointer;

    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        is_tx_required = 0;
        brightness     = (INT16)_ux_utility_short_get(data);
        _ux_utility_print_uint5("Brightness Control : UVC_SET_CUR. 0x%x ", (UINT)brightness, 0, 0, 0, 0);
        if ((brightness > brightness_setting.maximum_value) || (brightness < brightness_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);

            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            brightness_setting.current_value = brightness;
            uvcd_notify_pu_set(UVC_PU_BRIGHTNESS_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Brightness Control : UVC_GET_CUR.");
        brightness = brightness_setting.current_value;
        _ux_utility_short_put(data, (USHORT)brightness);
        break;

    case UVC_GET_INFO:
        dbg("Brightness Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_MIN:
        dbg("Brightness Control : GET_MIN.");
        _ux_utility_short_put(data, (USHORT)brightness_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Brightness Control : GET_MAX.");
        _ux_utility_short_put(data, (USHORT)brightness_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Brightness Control : GET_RES.");
        _ux_utility_short_put(data, 1);
        break;
    case UVC_GET_DEF:
        dbg("Brightness Control : GET_DEF.");
        _ux_utility_short_put(data, (USHORT)brightness_setting.default_value);
        break;
    default:
        _ux_utility_print_uint5("uvcd_pu_process_brightness_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_backlight_t backlight_setting =
{
    128,
    0,
    0,
    0
};

UINT32 uvcd_pu_get_backlight(uvcd_pu_backlight_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = backlight_setting.current_value;
        get->default_value = backlight_setting.default_value;
        get->maximum_value = backlight_setting.maximum_value;
        get->minimum_value = backlight_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_backlight(const uvcd_pu_backlight_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        backlight_setting.current_value = set->current_value;
        backlight_setting.default_value = set->default_value;
        backlight_setting.maximum_value = set->maximum_value;
        backlight_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_backlight_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 value          = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Backlight Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (UINT16)_ux_utility_short_get(data);
        if ((value > backlight_setting.maximum_value) ||
            (value < backlight_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            backlight_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_BACKLIGHT_COMPENSATION_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Backlight Control : GET_CUR.");
        _ux_utility_short_put(data, backlight_setting.current_value);
        break;
    case UVC_GET_MIN:
        dbg("Backlight : GET_MIN.");
        _ux_utility_short_put(data, backlight_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Backlight Control : GET_MAX.");
        _ux_utility_short_put(data, backlight_setting.maximum_value);
        break;
    case UVC_GET_RES:
        dbg("Backlight Control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("Backlight Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("Backlight Control : GET_DEF.");
        _ux_utility_short_put(data, backlight_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("pu_process_backlight_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_gamma_t gamma_setting =
{
    200,
    100,
    100,
    100
};

UINT32 uvcd_pu_get_gamma(uvcd_pu_gamma_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = gamma_setting.current_value;
        get->default_value = gamma_setting.default_value;
        get->maximum_value = gamma_setting.maximum_value;
        get->minimum_value = gamma_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_gamma(const uvcd_pu_gamma_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        gamma_setting.current_value = set->current_value;
        gamma_setting.default_value = set->default_value;
        gamma_setting.maximum_value = set->maximum_value;
        gamma_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}


static UINT pu_process_gamma_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 value          = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Gamma Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (UINT16)_ux_utility_short_get(data);
        if ((value > gamma_setting.maximum_value) ||
            (value < gamma_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            gamma_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_GAMMA_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;

    case UVC_GET_CUR:
        dbg("Gamma Control : GET_CUR.");
        _ux_utility_short_put(data, gamma_setting.current_value);
        break;
    case UVC_GET_MIN:
        dbg("Gamma Control : GET_MIN.");
        _ux_utility_short_put(data, gamma_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Gamma Control : GET_MAX.");
        _ux_utility_short_put(data, gamma_setting.maximum_value);
        break;
    case UVC_GET_RES:
        dbg("Gamma Control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("Gamma Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("Gamma Control : GET_DEF.");
        _ux_utility_short_put(data, gamma_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("pu_process_gamma_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_gain_t gain_setting =
{
    1,
    0,
    0,
    0
};

UINT32 uvcd_pu_get_gain(uvcd_pu_gain_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = gain_setting.current_value;
        get->default_value = gain_setting.default_value;
        get->maximum_value = gain_setting.maximum_value;
        get->minimum_value = gain_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_gain(const uvcd_pu_gain_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        gain_setting.current_value = set->current_value;
        gain_setting.default_value = set->default_value;
        gain_setting.maximum_value = set->maximum_value;
        gain_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}


static UINT pu_process_gain_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 value          = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Gain Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (UINT16)_ux_utility_short_get(data);
        if ((value > gain_setting.maximum_value) ||
            (value < gain_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            gain_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_GAIN_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Gain Control : GET_CUR.");
        _ux_utility_short_put(data, gain_setting.current_value);
        break;
    case UVC_GET_MIN:
        dbg("Gain Control : GET_MIN.");
        _ux_utility_short_put(data, gain_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Gain Control : GET_MAX.");
        _ux_utility_short_put(data, gain_setting.maximum_value);
        break;
    case UVC_GET_RES:
        dbg("Gain Control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("Gain Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("Gain Control : GET_DEF.");
        _ux_utility_short_put(data, gain_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("pu_process_gain_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_wbc_auto_t wbc_auto_setting =
{
    1,
    0,
    1,
    1
};

static UINT pu_process_wbc_auto_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("White Balance Component Auto Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = data[0];
        if ((value > wbc_auto_setting.maximum_value) || (value < wbc_auto_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            if (wbc_auto_setting.current_value != value) {
                // send status interrupt if capability is changed.
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };

                uvc_status.uvcd_status_type = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator  = (UINT8)processing_unit_id;
                uvc_status.uvcd_event       = 0;    // control change
                uvc_status.uvcd_selector    = UVC_PU_WHITE_BALANCE_COMP_CONTROL;
                uvc_status.uvcd_attribute   = 0x01; // control info change
                if (value == 1U) {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL;
                } else {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL;
                }
                uvc_status.uvcd_value_length = 1;
                if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                    // ignore this error
                }
            }

            wbc_auto_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_WHITE_BALANCE_COMP_AUTO_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;

    case UVC_GET_CUR:
        dbg("White Balance Component Auto Control : GET_CUR.");
        data[0] = wbc_auto_setting.current_value;
        break;

    case UVC_GET_INFO:
        dbg("White Balance Component Auto Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("White Balance Component Auto Control : GET_DEF.");
        data[0] = wbc_auto_setting.default_value;
        break;

    default:
        _ux_utility_print_uint5("pu_process_wbc_auto_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_wbc_t wbc_setting =
{
    100,
    100,
    0,
    0,
    0,
    0,
    0,
    0
};

static UINT32 uvcd_copy_wbc_setting(const uvcd_pu_wbc_t *src, uvcd_pu_wbc_t *dst)
{
    UINT32 uRet = UX_SUCCESS;

    if ((src == NULL) || (dst == NULL)) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        // do not use memcpy because misra-c concern
        dst->max_blue = src->max_blue;
        dst->max_red  = src->max_red;
        dst->min_blue = src->min_blue;
        dst->min_red  = src->min_red;
        dst->def_blue = src->def_blue;
        dst->def_red  = src->def_red;
        dst->cur_blue = src->cur_blue;
        dst->cur_red  = src->cur_red;
    }
    return uRet;
}

UINT32 uvcd_pu_get_wbc(uvcd_pu_wbc_t *get)
{
    UINT32 uRet = uvcd_copy_wbc_setting(&wbc_setting, get);

    return uRet;
}

UINT32 uvcd_pu_set_wbc(const uvcd_pu_wbc_t *set)
{
    UINT32 uRet = uvcd_copy_wbc_setting(set, &wbc_setting);

    return uRet;
}

static UINT pu_process_wbc_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 blue, red;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("White Balance Component Control : UVC_SET_CUR.");
        is_tx_required = 0;
        if (wbc_auto_setting.current_value == 1U) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_WRONG_STATE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;;
        }

        if (uRet == 0U) {
            blue = (UINT16)_ux_utility_short_get(data);
            data = &data[2];
            red  = (UINT16)_ux_utility_short_get(data);

            if ((blue > wbc_setting.max_blue) ||
                (blue < wbc_setting.min_blue) ||
                (red > wbc_setting.max_red) ||
                (red < wbc_setting.min_red)) {
                uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
                uvcd_stall_controll_endpoint();
                uRet = UX_TRANSFER_STALLED;
            } else {
                wbc_setting.cur_blue = blue;
                wbc_setting.cur_red  = red;
                uvcd_notify_pu_set(UVC_PU_WHITE_BALANCE_COMP_CONTROL);
                uRet = UX_SUCCESS;
            }
        }
        break;
    case UVC_GET_CUR:
        dbg("White Balance Component Control : GET_CUR.");
        _ux_utility_short_put(data, wbc_setting.cur_blue);
        data = &data[2];
        _ux_utility_short_put(data, wbc_setting.cur_red);
        break;
    case UVC_GET_MIN:
        dbg("White Balance Component Control : GET_MIN.");
        _ux_utility_short_put(data, wbc_setting.min_blue);
        data = &data[2];
        _ux_utility_short_put(data, wbc_setting.min_red);
        break;

    case UVC_GET_MAX:
        dbg("White Balance Component Control : GET_MAX.");
        _ux_utility_short_put(data, wbc_setting.max_blue);
        data = &data[2];
        _ux_utility_short_put(data, wbc_setting.max_red);
        break;
    case UVC_GET_RES:
        dbg("White Balance Component Control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("White Balance Component Control : GET_INFO.");
        {
            UINT32 capability = 0;
            if (wbc_auto_setting.current_value == 1U) {
                capability = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL;
            } else {
                capability = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL;
            }
            _ux_utility_long_put(data, capability);
        }
        break;

    case UVC_GET_DEF:
        dbg("White Balance Component Control : GET_DEF.");
        _ux_utility_short_put(data, wbc_setting.def_blue);
        data = &data[2];
        _ux_utility_short_put(data, wbc_setting.def_red);
        break;

    default:
        _ux_utility_print_uint5("pu_process_white_balance_component_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_digital_multiplier_t digital_multiplier_setting =
{
    255,
    100,
    100,
    100
};

static uvcd_pu_digital_multiplier_limit_t digital_multiplier_limit_setting =
{
    255,
    100,
    255,
    255  // temporary upper limit
};

static UINT pu_process_digital_multiplier_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 value          = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Digital Multiplier Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (UINT16)_ux_utility_short_get(data);
        if ((value > digital_multiplier_limit_setting.current_value) ||
            (value > digital_multiplier_setting.maximum_value) ||
            (value < digital_multiplier_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            digital_multiplier_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_DIGITAL_MULTIPLIER_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Digital Multiplier Control : GET_CUR.");
        _ux_utility_short_put(data, digital_multiplier_setting.current_value);
        break;
    case UVC_GET_MIN:
        dbg("Digital Multiplier Control : GET_MIN.");
        _ux_utility_short_put(data, digital_multiplier_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Digital Multiplier Control : GET_MAX.");
        _ux_utility_short_put(data, digital_multiplier_setting.maximum_value);
        break;
    case UVC_GET_RES:
        dbg("Digital Multiplier Control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("Digital Multiplier Control : GET_INFO.");
        _ux_utility_long_put(data, UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL);
        break;

    case UVC_GET_DEF:
        dbg("Digital Multiplier Control : GET_DEF.");
        _ux_utility_short_put(data, digital_multiplier_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("pu_process_digital_multiplier_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static UINT pu_process_digital_multiplier_limit_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 value          = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Digital Multiplier Limit Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = (UINT16)_ux_utility_short_get(data);
        if ((value > digital_multiplier_limit_setting.maximum_value) ||
            (value < digital_multiplier_limit_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            // if the limit multiplier value below current multiplier value
            // 1. adjust to match the new limit
            // 2. digital multiplier control send a control change event to notify the host of the adjustment

            if (digital_multiplier_setting.current_value > value) {
                // send status interrupt if capability is changed.
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };

                uvc_status.uvcd_status_type  = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator   = (UINT8)processing_unit_id;
                uvc_status.uvcd_event        = 0;    // control change
                uvc_status.uvcd_selector     = UVC_PU_DIGITAL_MULTIPLIER_CONTROL;
                uvc_status.uvcd_attribute    = 0x00; // control value
                uvc_status.uvcd_value        = (UINT32)value;
                uvc_status.uvcd_value_length = 1;

                if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                    // ignore this error
                }
            }

            digital_multiplier_setting.current_value       = value;
            digital_multiplier_limit_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;

    case UVC_GET_CUR:
        dbg("Digital Multiplier Limit Control : GET_CUR.");
        _ux_utility_short_put(data, (USHORT)digital_multiplier_limit_setting.current_value);
        break;
    case UVC_GET_MIN:
        dbg("Digital Multiplier Limit Control : GET_MIN.");
        _ux_utility_short_put(data, (USHORT)digital_multiplier_limit_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Digital Multiplier Limit Control : GET_MAX.");
        _ux_utility_short_put(data, (USHORT)digital_multiplier_limit_setting.maximum_value);
        break;
    case UVC_GET_RES:
        dbg("Digital Multiplier Limit Control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("Digital Multiplier Limit Control : GET_INFO.");
        _ux_utility_long_put(data, UVC_GET_SUPPORT | UVC_SET_SUPPORT);
        break;

    case UVC_GET_DEF:
        dbg("Digital Multiplier Limit Control : GET_DEF.");
        _ux_utility_short_put(data, (USHORT)digital_multiplier_limit_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("pu_process_digital_multiplier_limit_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_wbt_auto_t wbt_auto_setting =
{
    1,
    0,
    1,
    1
};

UINT32 uvcd_pu_get_wbt_auto(uvcd_pu_wbt_auto_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = wbt_auto_setting.current_value;
        get->default_value = wbt_auto_setting.default_value;
        get->maximum_value = wbt_auto_setting.maximum_value;
        get->minimum_value = wbt_auto_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_wbt_auto(const uvcd_pu_wbt_auto_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        wbt_auto_setting.current_value = set->current_value;
        wbt_auto_setting.default_value = set->default_value;
        wbt_auto_setting.maximum_value = set->maximum_value;
        wbt_auto_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_wbt_auto_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("White Balance Temperature Auto Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = data[0];
        if ((value > wbt_auto_setting.maximum_value) || (value < wbt_auto_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            if (wbt_auto_setting.current_value != value) {
                // send status interrupt if capability is changed.
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };

                uvc_status.uvcd_status_type = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator  = (UINT8)processing_unit_id;
                uvc_status.uvcd_event       = 0;    // control change
                uvc_status.uvcd_selector    = UVC_PU_WHITE_BALANCE_TEMP_CONTROL;
                uvc_status.uvcd_attribute   = 0x01; // control info change
                if (value == 1U) {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL;
                } else {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL;
                }
                uvc_status.uvcd_value_length = 1;
                if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                    // ignore this error
                }
            }

            wbt_auto_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_WHITE_BALANCE_TEMP_AUTO_CONTROL);

            uRet = UX_SUCCESS;
        }
        break;

    case UVC_GET_CUR:
        dbg("White Balance Temperature Auto Control : GET_CUR.");
        data[0] = wbt_auto_setting.current_value;
        break;

    case UVC_GET_INFO:
        dbg("White Balance Temperature Auto Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("White Balance Temperature Auto Control : GET_DEF.");
        data[0] = wbt_auto_setting.default_value;
        break;

    default:
        _ux_utility_print_uint5("pu_process_wbt_auto_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_wbt_t wbt_setting =
{
    6500,
    2800,
    2800,
    2800
};

UINT32 uvcd_pu_get_wbt(uvcd_pu_wbt_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = wbt_setting.current_value;
        get->default_value = wbt_setting.default_value;
        get->maximum_value = wbt_setting.maximum_value;
        get->minimum_value = wbt_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_wbt(const uvcd_pu_wbt_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        wbt_setting.current_value = set->current_value;
        wbt_setting.default_value = set->default_value;
        wbt_setting.maximum_value = set->maximum_value;
        wbt_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT uvcd_pu_process_wbt_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 value          = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("White Balance Temperature Control : UVC_SET_CUR.");
        is_tx_required = 0;
        if (wbt_auto_setting.current_value == 1U) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_WRONG_STATE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;;
        }

        if (uRet == 0U) {
            value = (UINT16)_ux_utility_short_get(data);
            if ((value > wbt_setting.maximum_value) ||
                (value < wbt_setting.minimum_value)) {
                uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
                uvcd_stall_controll_endpoint();
                uRet = UX_TRANSFER_STALLED;
            } else {
                // If a control is required to support Control Change events,
                // the event shall be sent for all SET_CUR operations.

                wbt_setting.current_value = value;
                uvcd_notify_pu_set(UVC_PU_WHITE_BALANCE_TEMP_CONTROL);
                uRet = UX_SUCCESS;
            }
        }
        break;
    case UVC_GET_CUR:
        dbg("White Balance Temperature Control : GET_CUR.");
        _ux_utility_short_put(data, wbt_setting.current_value);
        break;
    case UVC_GET_MIN:
        dbg("White Balance Temperature Control : GET_MIN.");
        _ux_utility_short_put(data, wbt_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("White Balance Temperature Control : GET_MAX.");
        _ux_utility_short_put(data, wbt_setting.maximum_value);
        break;
    case UVC_GET_RES:
        dbg("White Balance Temperature Control : GET_RES.");
        _ux_utility_short_put(data, 0x01);
        break;

    case UVC_GET_INFO:
        dbg("White Balance Temperature Control : GET_INFO.");
        {
            UINT32 capability = 0;
            if (wbt_auto_setting.current_value == 1U) {
                capability = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL;
            } else {
                capability = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL;
            }
            _ux_utility_long_put(data, capability);
        }

        break;

    case UVC_GET_DEF:
        dbg("White Balance Temperature Control : GET_DEF.");
        _ux_utility_short_put(data, wbt_setting.default_value);
        break;

    default:
        _ux_utility_print_uint5("uvcd_pu_process_wbt_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_contrast_t contrast_setting =
{
    256,
    0,
    64,
    64
};

UINT32 uvcd_pu_get_contrast(uvcd_pu_contrast_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = contrast_setting.current_value;
        get->default_value = contrast_setting.default_value;
        get->maximum_value = contrast_setting.maximum_value;
        get->minimum_value = contrast_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_contrast(const uvcd_pu_contrast_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        contrast_setting.current_value = set->current_value;
        contrast_setting.default_value = set->default_value;
        contrast_setting.maximum_value = set->maximum_value;
        contrast_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_contrast_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 contrast       = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Contrast Control : UVC_SET_CUR.");
        is_tx_required = 0;
        contrast       = (UINT16)_ux_utility_short_get(data);
        if ((contrast > contrast_setting.maximum_value) || (contrast < contrast_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            contrast_setting.current_value = contrast;
            uvcd_notify_pu_set(UVC_PU_CONTRAST_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Contrast Control : UVC_GET_CUR.");
        _ux_utility_short_put(data, contrast_setting.current_value);
        break;

    case UVC_GET_INFO:
        dbg("Contrast Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_MIN:
        dbg("Contrast Control : GET_MIN.");
        _ux_utility_short_put(data, contrast_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Contrast Control : GET_MAX.");
        _ux_utility_short_put(data, contrast_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Contrast Control : GET_RES.");
        _ux_utility_long_put(data, 1);
        break;
    case UVC_GET_DEF:
        dbg("Contrast Control : GET_DEF.");
        _ux_utility_short_put(data, contrast_setting.default_value);
        break;
    default:
        _ux_utility_print_uint5("pu_process_contrast_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_hue_auto_t hue_auto_setting =
{
    1,
    0,
    0,
    0
};

static UINT pu_process_hue_auto_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 value           = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Hue Auto Control : UVC_SET_CUR.");
        is_tx_required = 0;
        value          = data[0];
        if ((value > hue_auto_setting.maximum_value) || (value < hue_auto_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            if (hue_auto_setting.current_value != value) {
                // send status interrupt if capability is changed.
                UX_SLAVE_CLASS_UVC *uvc_ctx          = uvcd_get_context();
                UX_SLAVE_CLASS_UVC_STATUS uvc_status = { 0 };

                uvc_status.uvcd_status_type = UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL;
                uvc_status.uvcd_originator  = (UINT8)processing_unit_id;
                uvc_status.uvcd_event       = 0;    // control change
                uvc_status.uvcd_selector    = UVC_PU_HUE_CONTROL;
                uvc_status.uvcd_attribute   = 0x01; // control info change
                if (value == 1U) {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL;
                } else {
                    uvc_status.uvcd_value = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL;
                }
                uvc_status.uvcd_value_length = 1;
                if (_ux_device_class_uvc_status_set(uvc_ctx, &uvc_status) != 0U) {
                    // ignore this error
                }
            }

            hue_auto_setting.current_value = value;
            uvcd_notify_pu_set(UVC_PU_HUE_AUTO_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Hue Auto Control : GET_CUR.");
        data[0] = hue_auto_setting.current_value;
        break;

    case UVC_GET_INFO:
        dbg("Hue Auto Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("Hue Auto Control : GET_DEF.");
        data[0] = hue_auto_setting.default_value;
        break;

    default:
        _ux_utility_print_uint5("pu_process_hue_auto_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_hue_t hue_setting =
{
    128,
    -128,
    0,
    0
};

UINT32 uvcd_pu_get_hue(uvcd_pu_hue_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = hue_setting.current_value;
        get->default_value = hue_setting.default_value;
        get->maximum_value = hue_setting.maximum_value;
        get->minimum_value = hue_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_hue(const uvcd_pu_hue_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        hue_setting.current_value = set->current_value;
        hue_setting.default_value = set->default_value;
        hue_setting.maximum_value = set->maximum_value;
        hue_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_hue_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    INT16 hue             = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Hue Control : UVC_SET_CUR.");
        is_tx_required = 0;
        if (hue_auto_setting.current_value == 1U) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_WRONG_STATE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;;
        }

        if (uRet == 0U) {
            hue = (INT16)_ux_utility_short_get(data);
            if ((hue > hue_setting.maximum_value) || (hue < hue_setting.minimum_value)) {
                uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
                uvcd_stall_controll_endpoint();
                uRet = UX_TRANSFER_STALLED;
            } else {
                hue_setting.current_value = hue;
                uvcd_notify_pu_set(UVC_PU_HUE_CONTROL);
                uRet = UX_SUCCESS;
            }
        }
        break;
    case UVC_GET_CUR:
        dbg("Hue Control : UVC_GET_CUR.");
        _ux_utility_short_put(data, (USHORT)hue_setting.current_value);
        break;

    case UVC_GET_INFO:
        dbg("Hue Control : GET_INFO.");
        {
            UINT32 capability = 0;
            if (hue_auto_setting.current_value == 1U) {
                capability = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_DISABLE_DUE_TO_AUTOMATIC_MODE | UVC_AUTOUPDATE_CONTROL;
            } else {
                capability = UVC_GET_SUPPORT | UVC_SET_SUPPORT | UVC_AUTOUPDATE_CONTROL;
            }
            _ux_utility_long_put(data, capability);
        }
        break;

    case UVC_GET_MIN:
        dbg("Hue Control : GET_MIN.");
        _ux_utility_short_put(data, (USHORT)hue_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Hue Control : GET_MAX.");
        _ux_utility_short_put(data, (USHORT)hue_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Hue Control : GET_RES.");
        _ux_utility_long_put(data, 1);
        break;
    case UVC_GET_DEF:
        dbg("Hue Control : GET_DEF.");
        _ux_utility_short_put(data, (USHORT)hue_setting.default_value);
        break;
    case UVC_GET_LEN:
        dbg("Hue Control : GET_LEN.");
        _ux_utility_short_put(data, 2);
        break;
    default:
        _ux_utility_print_uint5("pu_process_hue_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_sharpness_t sharpness_setting =
{
    5,
    0,
    0,
    0
};

UINT32 uvcd_pu_get_sharpness(uvcd_pu_sharpness_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = sharpness_setting.current_value;
        get->default_value = sharpness_setting.default_value;
        get->maximum_value = sharpness_setting.maximum_value;
        get->minimum_value = sharpness_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_sharpness(const uvcd_pu_sharpness_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        sharpness_setting.current_value = set->current_value;
        sharpness_setting.default_value = set->default_value;
        sharpness_setting.maximum_value = set->maximum_value;
        sharpness_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_sharpness_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 sharpness      = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Sharpness Control : UVC_SET_CUR.");
        is_tx_required = 0;
        sharpness      = (UINT16)_ux_utility_short_get(data);
        if ((sharpness > sharpness_setting.maximum_value) || (sharpness < sharpness_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            sharpness_setting.current_value = sharpness;
            uvcd_notify_pu_set(UVC_PU_SHARPNESS_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Sharpness Control : UVC_GET_CUR.");
        _ux_utility_short_put(data, sharpness_setting.current_value);
        break;

    case UVC_GET_INFO:
        dbg("Sharpness Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_MIN:
        dbg("Sharpness Control : GET_MIN.");
        _ux_utility_short_put(data, sharpness_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Sharpness Control : GET_MAX.");
        _ux_utility_short_put(data, sharpness_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Sharpness Control : GET_RES.");
        _ux_utility_long_put(data, 1);
        break;

    case UVC_GET_DEF:
        dbg("Sharpness Control : GET_DEF.");
        _ux_utility_short_put(data, sharpness_setting.default_value);
        break;

    case UVC_GET_LEN:
        dbg("Sharpness Control : GET_LEN.");
        _ux_utility_short_put(data, 2);
        break;

    default:
        _ux_utility_print_uint5("uvc_pu_process_sharpness_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_saturation_t saturation_setting =
{
    128,
    0,
    0,
    0
};

UINT32 uvcd_pu_get_saturation(uvcd_pu_saturation_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = saturation_setting.current_value;
        get->default_value = saturation_setting.default_value;
        get->maximum_value = saturation_setting.maximum_value;
        get->minimum_value = saturation_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_saturation(const uvcd_pu_saturation_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        saturation_setting.current_value = set->current_value;
        saturation_setting.default_value = set->default_value;
        saturation_setting.maximum_value = set->maximum_value;
        saturation_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_saturation_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT16 saturation     = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("Saturation Control : UVC_SET_CUR.");
        is_tx_required = 0;
        saturation     = (UINT16)_ux_utility_short_get(data);
        if ((saturation > saturation_setting.maximum_value) || (saturation < saturation_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            saturation_setting.current_value = saturation;
            uvcd_notify_pu_set(UVC_PU_SATURATION_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("Saturation Control : UVC_GET_CUR.");
        _ux_utility_short_put(data, saturation_setting.current_value);
        break;

    case UVC_GET_INFO:
        dbg("Saturation Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_MIN:
        dbg("Saturation Control : GET_MIN.");
        _ux_utility_short_put(data, saturation_setting.minimum_value);
        break;

    case UVC_GET_MAX:
        dbg("Saturation Control : GET_MAX.");
        _ux_utility_short_put(data, saturation_setting.maximum_value);
        break;

    case UVC_GET_RES:
        dbg("Saturation Control : GET_RES.");
        _ux_utility_short_put(data, 1);
        break;
    case UVC_GET_DEF:
        dbg("Saturation Control : GET_DEF.");
        _ux_utility_short_put(data, saturation_setting.default_value);
        break;
    case UVC_GET_LEN:
        dbg("Saturation Control : GET_LEN.");
        _ux_utility_short_put(data, 2);
        break;
    default:
        _ux_utility_print_uint5("pu_process_saturation_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

static uvcd_pu_power_line_t powerline_setting =
{
    UVC_PU_POWERLINE_60HZ,
    UVC_PU_POWERLINE_DISABLE,
    UVC_PU_POWERLINE_60HZ,
    UVC_PU_POWERLINE_60HZ
};

UINT32 uvcd_pu_get_powerline(uvcd_pu_power_line_t *get)
{
    UINT32 uRet = UX_SUCCESS;

    if (get == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        get->current_value = powerline_setting.current_value;
        get->default_value = powerline_setting.default_value;
        get->maximum_value = powerline_setting.maximum_value;
        get->minimum_value = powerline_setting.minimum_value;
    }
    return uRet;
}

UINT32 uvcd_pu_set_powerline(const uvcd_pu_power_line_t *set)
{
    UINT32 uRet = UX_SUCCESS;

    if (set == NULL) {
        uRet = UX_MEMORY_INSUFFICIENT;
    } else {
        powerline_setting.current_value = set->current_value;
        powerline_setting.default_value = set->default_value;
        powerline_setting.maximum_value = set->maximum_value;
        powerline_setting.minimum_value = set->minimum_value;
    }
    return uRet;
}

static UINT pu_process_powerline_control(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 powerline       = 0;
    UINT8 *data           = transfer->ux_slave_transfer_request_data_pointer;
    UINT32 is_tx_required = 1;
    UINT32 uRet           = UX_SUCCESS;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
        dbg("PowerLine Control : UVC_SET_CUR.");
        is_tx_required = 0;
        powerline      = data[0];
        if ((powerline > powerline_setting.maximum_value) || (powerline < powerline_setting.minimum_value)) {
            uvcd_set_request_error_code(UVC_VC_ER_CD_OUT_OF_RANGE);
            uvcd_stall_controll_endpoint();
            uRet = UX_TRANSFER_STALLED;
        } else {
            powerline_setting.current_value = powerline;
            uvcd_notify_pu_set(UVC_PU_POWER_LINE_FREQUENCY_CONTROL);
            uRet = UX_SUCCESS;
        }
        break;
    case UVC_GET_CUR:
        dbg("PowerLine Control : GET_CUR.");
        data[0] = powerline_setting.current_value;
        break;

    case UVC_GET_MIN:
        dbg("PowerLine Control : GET_MIN.");
        data[0] = powerline_setting.minimum_value;
        break;

    case UVC_GET_MAX:
        dbg("PowerLine Control : GET_MAX.");
        data[0] = powerline_setting.maximum_value;
        break;
    case UVC_GET_RES:
        dbg("PowerLine Control : GET_RES.");
        data[0] = 1;
        break;

    case UVC_GET_INFO:
        dbg("PowerLine Control : GET_INFO.");
        _ux_utility_long_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
        break;

    case UVC_GET_DEF:
        dbg("PowerLine Control : GET_DEF.");
        data[0] = powerline_setting.default_value;
        break;

    default:
        _ux_utility_print_uint5("pu_process_powerline_control() : Unknown bRequest = 0x%X", sreq->bRequest, 0, 0, 0, 0);
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

UINT uvcd_process_request_pu(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq)
{
    UINT32 cs_id = (((UINT32)sreq->wValue >> 8U)) & 0x0FFU;
    UINT uRet;

    switch (cs_id) {
    case UVC_PU_BRIGHTNESS_CONTROL:
        dbg("Brightness Control Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_BRIGHTNESS_MASK) != 0U) {
            uRet = pu_process_brightness_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_CONTRAST_CONTROL:
        dbg("Contrast Control Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_CONTRAST_MASK) != 0U) {
            uRet = pu_process_contrast_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_HUE_AUTO_CONTROL:
        dbg("Hue Auto Control Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_HUE_AUTO_MASK) != 0U) {
            uRet = pu_process_hue_auto_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_HUE_CONTROL:
        dbg("Hue Control Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_HUE_MASK) != 0U) {
            uRet = pu_process_hue_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_SATURATION_CONTROL:
        dbg("Saturation Control Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_SATURATION_MASK) != 0U) {
            uRet = pu_process_saturation_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_SHARPNESS_CONTROL:
        dbg("Sharpness Control Selector.");
        // Check if upper layer support the control
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_SHARPNESS_MASK) != 0U) {
            uRet = pu_process_sharpness_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
        dbg("PowerLine Control Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_POWER_LINE_FREQUENCY_MASK) != 0U) {
            uRet = pu_process_powerline_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_WHITE_BALANCE_TEMP_AUTO_CONTROL:
        dbg("White Balance Temperature Auto Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_WB_TEMPERATURE_AUTO_MASK) != 0U) {
            uRet = pu_process_wbt_auto_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_WHITE_BALANCE_TEMP_CONTROL:
        dbg("White Balance Temperature Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_WB_TEMPERATURE_MASK) != 0U) {
            uRet = uvcd_pu_process_wbt_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_BACKLIGHT_COMPENSATION_CONTROL:
        dbg("Backlight Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_BACKLIGHT_COMPENSATION_MASK) != 0U) {
            uRet = pu_process_backlight_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_GAMMA_CONTROL:
        dbg("Gamma Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_GAMMA_MASK) != 0U) {
            uRet = pu_process_gamma_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_GAIN_CONTROL:
        dbg("Gain Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_GAIN_MASK) != 0U) {
            uRet = pu_process_gain_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_WHITE_BALANCE_COMP_AUTO_CONTROL:
        dbg("White Balance Component Auto Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_WB_COMPONENT_AUTO_MASK) != 0U) {
            uRet = pu_process_wbc_auto_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_WHITE_BALANCE_COMP_CONTROL:
        dbg("White Balance Component Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_WB_COMPONENT_MASK) != 0U) {
            uRet = pu_process_wbc_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_DIGITAL_MULTIPLIER_CONTROL:
        dbg("Digital Multiplier Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_DIGITAL_MULT_MASK) != 0U) {
            uRet = pu_process_digital_multiplier_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    case UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        dbg("Digital Multiplier Limit Selector.");
        if ((uvcd_get_pu_attribute() & UVC_PU_CONTROL_DIGITAL_MULT_LIMIT_MASK) != 0U) {
            uRet = pu_process_digital_multiplier_limit_control(transfer, sreq);
        } else {
            uRet = uvcd_process_unsupported_control();
        }
        break;

    default:
        _ux_utility_print_uint5("Unknown PU Control Selector(0x%X)", cs_id, 0, 0, 0, 0);
        uRet = uvcd_process_unsupported_control();
        break;
    }
    return uRet;
}

