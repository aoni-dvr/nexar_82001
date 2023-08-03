/**
 *  @file ux_device_class_uvc.h
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
 *  @details This file contains all the header and extern functions used by the USBX device uvc class.
 */

#ifndef ux_device_class_uvc_h
#define ux_device_class_uvc_h

#include <StdUSB.h>
#include <StdUVC.h>

#define UX_SLAVE_CLASS_UVC_CLASS             0x0E

#define UVC_FRAME_TYPE_OFFSET_FS             168
#define UVC_FRAME_TYPE_OFFSET_HS             178
#define UVC_FRAME_LENGTH                     0x32

#define UVC_FORMAT_INDEX_MJPEG               0x01U

#define UVC_FRAME_INDEX_320x240              0x01U
#define UVC_FRAME_INDEX_640x480              0x02U
#define UVC_FRAME_INDEX_1280x720             0x03U
#define UVC_FRAME_INDEX_1920x1080            0x04U

/* UVCD Max Requests Number */
#define UVC_MAX_REQUEST_NUM                  20

// -------------------------------------------------
// ------ Input Terminal ---------------------------
// -------------------------------------------------
#define UVC_IT_CONTROL_SIZE                  0x03

#define UVC_AE_MODE_MANUAL                   0x01U
#define UVC_AE_MODE_AUTO                     0x02U
#define UVC_AE_MODE_SHUTTER_PRIORITY         0x04U
#define UVC_AE_MODE_APERTURE_PRIORITY        0x08U
#define UVC_AE_MODE_W_LENGTH                 0x01U

// -------------------------------------------------
// ------ Processing Uint --------------------------
// -------------------------------------------------
#define UVC_PU_CONTROL_SIZE                  0x02

#define UVC_PU_POWERLINE_DISABLE             0x0U
#define UVC_PU_POWERLINE_50HZ                0x01U
#define UVC_PU_POWERLINE_60HZ                0x02U

#define UDC_UVC_EVENT_CLEAR_STALL            0x01U
#define UDC_UVC_EVENT_SET_FEATURE            0x02U

#define UX_DEVICE_CLASS_UVC_MAX_STATUS_QUEUE 16U

/* Define Uvc Status Type*/
#define UX_DEVICE_CLASS_UVC_SI_RESERVED      0x0U
#define UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL 0x1U
#define UX_DEVICE_CLASS_UVC_SI_VIDEO_STREAM  0x2U

/* Define Uvc Status Interrupt Data Format.  */
#define UX_DEVICE_CLASS_UVC_SI_STATUS_TYPE   0x00U
#define UX_DEVICE_CLASS_UVC_SI_ORIGINATOR    0x01U
#define UX_DEVICE_CLASS_UVC_SI_VC_EVENT      0x02U
#define UX_DEVICE_CLASS_UVC_SI_VC_SELECTOR   0x03U
#define UX_DEVICE_CLASS_UVC_SI_VC_ATTRIBUTE  0x04U
#define UX_DEVICE_CLASS_UVC_SI_VC_VALUE      0x05U
#define UX_DEVICE_CLASS_UVC_SI_VS_EVENT      0x02U
#define UX_DEVICE_CLASS_UVC_SI_VS_VALUE      0x03U

/* Define Slave Video Class Calling Parameter structure */

typedef struct
{
    UINT32 vs_num;
}UVCD_VS_INSTANCE_INFO;

typedef struct UVCD_VS_INSTANCE_STRUCT
{
    UINT32                          vs_id;
    UX_SLAVE_INTERFACE *            vs;
    struct UVCD_VS_INSTANCE_STRUCT *next_vs;
}UVCD_VS_INSTANCE;

/* Define Slave uvc Class structure.  */

typedef struct
{
    UINT8  uvcd_status_type;
    UINT8  uvcd_originator;
    UINT8  uvcd_event;
    UINT8  uvcd_selector;
    UINT8  uvcd_attribute;
    UINT32 uvcd_value;
    UINT16 uvcd_value_length;
} UX_SLAVE_CLASS_UVC_STATUS;

typedef struct
{
    UX_SLAVE_INTERFACE *       ux_slave_class_uvc_interface;
    TX_SEMAPHORE               class_thread_semaphore;
    VOID *                     event_thread_stack;
    TX_SEMAPHORE               event_thread_semaphore;
    TX_THREAD                  event_thread;
    UINT32                     event_id;
    UINT32                     event_endpoint_id;
    TX_MUTEX                   request_mutex;
    UX_SLAVE_REQUEST_WRAPPER * request_pool;
    ULONG                      request_num;
    UX_SLAVE_INTERFACE *       vc_interface;
    UX_SLAVE_CLASS_UVC_STATUS *uvcd_status_array;
    UX_SLAVE_CLASS_UVC_STATUS *uvcd_status_array_head;
    UX_SLAVE_CLASS_UVC_STATUS *uvcd_status_array_tail;
    UX_SLAVE_CLASS_UVC_STATUS *uvcd_status_array_end;
    UVCD_VS_INSTANCE_INFO      vs_info;
    UVCD_VS_INSTANCE *         first_vs_instance;
} UX_SLAVE_CLASS_UVC;

typedef struct  {
    UINT16 bmHint;
    UINT8  bFormatIndex;
    UINT8  bFrameIndex;
    UINT32 dwFrameInterval;
    UINT16 wKeyFrameRate;
    UINT16 wPFrameRate;
    UINT16 wCompQuality;
    UINT16 wCompWindowSize;
    UINT16 wDelay;
    UINT32 dwMaxVideoFrameSize;
    UINT32 dwMaxPayloadTransferSize;
} uvcd_probe_control_t;

typedef struct  {
    UINT8  bFormatIndex;
    UINT8  bFrameIndex;
    UINT8  bCompressionIndex;
    UINT32 dwMaxVideoFrameSize;
    UINT32 dwMaxPayloadTransferSize;
} uvcd_still_probe_control_t;

typedef struct  {
    UINT32 width;
    UINT32 height;
    UINT32 mjpeg_quality;
    UINT32 framerate;
    UINT32 format;
} uvcd_host_config_t;

typedef struct {
    INT16 maximum_value;
    INT16 minimum_value;
    INT16 default_value;
    INT16 current_value;
} uvcd_pu_brightness_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_contrast_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_backlight_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_gamma_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_gain_t;

// White Balance Component, Auto Control
typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_pu_wbc_auto_t;

// White Balance Component Control
typedef struct {
    UINT16 max_blue;
    UINT16 max_red;
    UINT16 min_blue;
    UINT16 min_red;
    UINT16 def_blue;
    UINT16 def_red;
    UINT16 cur_blue;
    UINT16 cur_red;
} uvcd_pu_wbc_t;

// White Balance Temperature, Auto Control
typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_pu_wbt_auto_t;

// White Balance Temperature, Auto Control
typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_wbt_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_digital_multiplier_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_digital_multiplier_limit_t;

typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_pu_hue_auto_t;

typedef struct {
    INT16 maximum_value;
    INT16 minimum_value;
    INT16 default_value;
    INT16 current_value;
} uvcd_pu_hue_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_sharpness_t;

typedef struct {
    UINT16 maximum_value;
    UINT16 minimum_value;
    UINT16 default_value;
    UINT16 current_value;
} uvcd_pu_saturation_t;

typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_pu_power_line_t;

typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_it_privacy_t;

typedef struct {
    INT32 max_pan;
    INT32 max_tilt;
    INT32 min_pan;
    INT32 min_tilt;
    INT32 def_pan;
    INT32 def_tilt;
    INT32 cur_pan;
    INT32 cur_tilt;
} uvcd_it_pantilt_absolute_t;

typedef struct {
    INT16 maximum_value;
    INT16 minimum_value;
    INT16 default_value;
    INT16 current_value;
} uvcd_it_roll_absolute_t;

typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_it_auto_exposure_priority_t;

typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_it_scanning_mode_t;

typedef struct {
    INT16 maximum_value;
    INT16 minimum_value;
    INT16 default_value;
    INT16 current_value;
} uvcd_it_iris_absolute_t;

typedef struct {
    INT16 maximum_value;
    INT16 minimum_value;
    INT16 default_value;
    INT16 current_value;
} uvcd_it_zoom_absolute_t;

typedef struct {
    INT16 maximum_value;
    INT16 minimum_value;
    INT16 default_value;
    INT16 current_value;
} uvcd_it_focus_absolute_t;

typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_it_focus_auto_t;

typedef struct {
    UINT32 maximum_value;
    UINT32 minimum_value;
    UINT32 default_value;
    UINT32 current_value;
} uvcd_it_exposure_time_absoulte_t;

typedef struct {
    UINT8 maximum_value;
    UINT8 minimum_value;
    UINT8 default_value;
    UINT8 current_value;
} uvcd_it_auto_exposure_mode_t;


#define UVC_GET_SUPPORT                   0x01U
#define UVC_SET_SUPPORT                   0x02U
#define UVC_DISABLE_DUE_TO_AUTOMATIC_MODE 0x04U
#define UVC_AUTOUPDATE_CONTROL            0x08U
#define UVC_ASYNCHRONOUS_CONTROL          0x10U

typedef struct  {
    UINT32 (*encode_start)(UINT32 index);
    UINT32 (*encode_stop)(UINT32 index);
    UINT32 (*pu_set)(UINT32 index);
    UINT32 (*it_set)(UINT32 index);
    UINT32 (*xu_handler)(UINT32 selector, UINT8 *buf, UINT32 buf_size, UINT32 request_length);
} uvcd_event_callback_t;

// global variables
#define EVENT_THREAD_STACK_SIZE (4096U)
#define EVENT_THREAD_PRIORITY   (71)

#define MAIN_THREAD_STACK_SIZE  (8U * 1024U)
#define MAIN_THREAD_PRIORITY    (71)

#define UVCD_MAX_VS_INSTANCE    5U
extern UX_SLAVE_CLASS_UVC uvcd_class_memory;
extern UINT8 uvcd_main_thread_stack_memory[MAIN_THREAD_STACK_SIZE];
extern UINT8 uvcd_event_thread_stack_memory[EVENT_THREAD_STACK_SIZE];
extern UX_SLAVE_REQUEST_WRAPPER uvcd_wrapper_pool_memory[UVC_MAX_REQUEST_NUM];
extern UX_SLAVE_CLASS_UVC_STATUS uvcd_status_memory[UX_DEVICE_CLASS_UVC_MAX_STATUS_QUEUE];
extern UVCD_VS_INSTANCE uvcd_vs_memory[UVCD_MAX_VS_INSTANCE];

VOID  _ux_device_class_uvc_thread(ULONG uvc_class);
UINT    _ux_device_class_uvc_initialize(const UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_reset(const UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_activate(const UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_deactivate(const UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_control_request(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_uvc_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    _ux_device_class_uvc_still_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    _ux_device_class_uvc_commit_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    _ux_device_class_uvc_still_commit_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    _ux_device_class_uvc_payload_send(UINT8 *buffer, UINT32 size, UINT32 timeout, UINT32 nocopy);
UINT32 _ux_device_class_uvc_multi_payload_send(UINT32 id, UINT8 *buffer, UINT32 size, UINT32 timeout, UINT32 nocopy);
UINT32  _ux_device_class_uvc_get_max_xfer_length(UINT32 vs_id);
UX_SLAVE_TRANSFER* _ux_device_class_uvc_allocate_transfer_request(UINT32 vs_id);
UINT    _ux_device_class_uvc_free_transfer_request(const UX_SLAVE_TRANSFER *transfer_request);
UINT    _ux_device_class_uvc_fill_iso_transfer(UX_SLAVE_TRANSFER *                     transfer_request,
                                               VOID *                                  iso_req_head,
                                               ULONG                                   iso_req_num,
                                               ULONG                                   timeout,
                                               ux_slave_transfer_request_completion_fn compl_fn);
UINT    _ux_device_class_uvc_submit_iso_transfer(UX_SLAVE_TRANSFER *transfer_request);
VOID    _uvcd_event_thread(ULONG uvc_class);
UINT  _ux_device_class_uvc_status_get(UX_SLAVE_CLASS_UVC *       uvc,
                                      UX_SLAVE_CLASS_UVC_STATUS *uvc_status);
UINT  _ux_device_class_uvc_status_set(UX_SLAVE_CLASS_UVC *       uvc,
                                      const UX_SLAVE_CLASS_UVC_STATUS *uvc_status);

void  uvcd_config_probe_control(UCHAR *data_pointer, ULONG request_length);
uvcd_probe_control_t *uvcd_get_probe_control(void);
void  uvcd_config_mjpeg_resolution(UINT8 format_idx, UINT8 frame_idx);
void  uvcd_copy_probe_control(const uvcd_probe_control_t *control, UINT8 *out);
//void  uvcd_copy_still_probe_control(uvcd_still_probe_control_t *control, UINT8 *out);
void  uvcd_decode_probe_control(UINT8 *data, uvcd_probe_control_t *control);
void  uvcd_decode_still_probe_control(UINT8 *data, uvcd_still_probe_control_t *control);
void  uvcd_send_control_null_packet(UX_SLAVE_TRANSFER *transfer);
void  uvcd_stall_controll_endpoint(void);
UX_SLAVE_CLASS_UVC *uvcd_get_context(void);
UX_SLAVE_CLASS     *uvcd_get_class(void);
void  uvcd_set_context(UX_SLAVE_CLASS_UVC *ctx);
void  uvcd_set_class(UX_SLAVE_CLASS *c);
void  uvcd_notify_encode_start(UINT32 idx);
void  uvcd_notify_encode_stop(UINT32 idx);
void  uvcd_notify_pu_set(UINT32 idx);
void  uvcd_notify_it_set(UINT32 idx);
UINT32 uvcd_notify_xu_handler(UINT32 code, UINT8 *buf, UINT32 buf_size, UINT32 request_length);

// utility functions exposed
UINT32 uvcd_get_processing_unit_id(void);
UINT32 uvcd_get_input_terminal_id(void);

UINT32 uvcd_process_unsupported_control(void);
UINT uvcd_process_request_pu(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq);
UINT uvcd_process_request_it(UX_SLAVE_TRANSFER *transfer, const USB_SETUP_REQUEST_S *sreq);
void uvcd_set_request_error_code(UINT32 value);

UINT32 uvcd_get_mjpeg_quality(void);
UINT32 uvcd_set_mjpeg_quality(UINT32 quality);
UINT32 uvcd_register_callback(const uvcd_event_callback_t *cbs);
UINT32 uvcd_get_host_config(uvcd_host_config_t *config);

UINT32 uvcd_set_input_terminal_id(UINT32 id);
UINT32 uvcd_set_processing_unit_id(UINT32 id);
UINT32 uvcd_set_extension_unit_id(UINT32 id);

UINT32 uvcd_pu_get_brightness(uvcd_pu_brightness_t *get);
UINT32 uvcd_pu_set_brightness(const uvcd_pu_brightness_t *set);
UINT32 uvcd_pu_get_contrast(uvcd_pu_contrast_t *get);
UINT32 uvcd_pu_set_contrast(const uvcd_pu_contrast_t *set);
UINT32 uvcd_pu_get_hue(uvcd_pu_hue_t *get);
UINT32 uvcd_pu_set_hue(const uvcd_pu_hue_t *set);
UINT32 uvcd_pu_get_sharpness(uvcd_pu_sharpness_t *get);
UINT32 uvcd_pu_set_sharpness(const uvcd_pu_sharpness_t *set);
UINT32 uvcd_pu_get_saturation(uvcd_pu_saturation_t *get);
UINT32 uvcd_pu_set_saturation(const uvcd_pu_saturation_t *set);
UINT32 uvcd_pu_get_powerline(uvcd_pu_power_line_t *get);
UINT32 uvcd_pu_set_powerline(const uvcd_pu_power_line_t *set);
UINT32 uvcd_pu_get_wbc(uvcd_pu_wbc_t *get);
UINT32 uvcd_pu_set_wbc(const uvcd_pu_wbc_t *set);
UINT32 uvcd_pu_get_wbt_auto(uvcd_pu_wbt_auto_t *get);
UINT32 uvcd_pu_set_wbt_auto(const uvcd_pu_wbt_auto_t *set);
UINT32 uvcd_pu_get_backlight(uvcd_pu_backlight_t *get);
UINT32 uvcd_pu_set_backlight(const uvcd_pu_backlight_t *set);
UINT32 uvcd_pu_get_gain(uvcd_pu_gain_t *get);
UINT32 uvcd_pu_set_gain(const uvcd_pu_gain_t *set);
UINT32 uvcd_pu_get_gamma(uvcd_pu_gamma_t *get);
UINT32 uvcd_pu_set_gamma(const uvcd_pu_gamma_t *set);
UINT32 uvcd_pu_get_wbt(uvcd_pu_wbt_t *get);
UINT32 uvcd_pu_set_wbt(const uvcd_pu_wbt_t *set);

UINT32 uvcd_set_it_pantile_abs(const uvcd_it_pantilt_absolute_t *s);
UINT32 uvcd_get_it_pantile_abs(uvcd_it_pantilt_absolute_t *s);

UINT32 uvcd_get_probe_max_payload_xfer_size(void);
void   uvcd_set_probe_max_payload_xfer_size(UINT32 size);

UINT32 uvcd_set_it_attribute(UINT32 attribute);
UINT32 uvcd_get_it_attribute(void);
UINT32 uvcd_set_pu_attribute(UINT32 attribute);
UINT32 uvcd_get_pu_attribute(void);

UX_SLAVE_CLASS_UVC* uvc_class_void_2_uvc(const VOID *pVoid);
UX_SLAVE_INTERFACE* uvc_class_void_2_interface(const VOID *pVoid);

#endif

