/**
 *  @file ux_device_class_uvc.h
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
 *  @details This file contains all the header and extern functions used by the USBX device uvc class.
 */

#ifndef ux_device_class_uvc_h
#define ux_device_class_uvc_h

#ifndef STD_USB_H
#include <StdUSB.h>
#endif
#ifndef STD_USB_VIDEO_CLASS_H
#include <StdUVC.h>
#endif

#define UX_SLAVE_CLASS_UVC_CLASS             0x0E

#define UVC_FRAME_TYPE_OFFSET_FS             168
#define UVC_FRAME_TYPE_OFFSET_HS             178
#define UVC_FRAME_LENGTH                     0x32

#define UVC_MAX_STREAM_NUMBER                (0x03UL)

/* UVCD Max Requests Number */
#define UVC_MAX_REQUEST_NUM                  20

// -------------------------------------------------
// ------ Input Terminal ---------------------------
// -------------------------------------------------

#define UVC_AE_MODE_MANUAL                   0x01U
#define UVC_AE_MODE_AUTO                     0x02U
#define UVC_AE_MODE_SHUTTER_PRIORITY         0x04U
#define UVC_AE_MODE_APERTURE_PRIORITY        0x08U
#define UVC_AE_MODE_W_LENGTH                 0x01U

// -------------------------------------------------
// ------ Processing Uint --------------------------
// -------------------------------------------------

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

typedef struct {
    UINT32 vs_num;
} UVCD_VS_INSTANCE_INFO;

typedef struct UVCD_VS_INSTANCE_STRUCT {
    UINT32                          stream_id;
    UX_SLAVE_INTERFACE *            ux_interface;
    struct UVCD_VS_INSTANCE_STRUCT *next;
} UVCD_VS_INSTANCE;

/* Define Slave uvc Class structure.  */

typedef struct {
    UINT8  status_type;
    UINT8  originator;
    UINT8  event;
    UINT8  selector;
    UINT8  attribute;
    UINT32 value;
    UINT16 value_length;
} UX_SLAVE_CLASS_UVC_STATUS;

typedef struct {
    UX_SLAVE_INTERFACE *       ux_interface;
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
    UX_SLAVE_CLASS_UVC_STATUS *status_array;
    UX_SLAVE_CLASS_UVC_STATUS *status_array_head;
    UX_SLAVE_CLASS_UVC_STATUS *status_array_tail;
    UX_SLAVE_CLASS_UVC_STATUS *status_array_end;
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
    UINT mjpeg_quality; //!< MJPEG quality level set by Host
    UINT framerate;     //!< Video framerate set by Host
    UINT format_index;  //!< video format index (in USB descriptor) set by Host
    UINT frame_index;   //!< video frame index (in USB descriptor) set by Host
} uvcd_host_config_t;

#define UVC_GET_SUPPORT                   0x01U
#define UVC_SET_SUPPORT                   0x02U
#define UVC_DISABLE_DUE_TO_AUTOMATIC_MODE 0x04U
#define UVC_AUTOUPDATE_CONTROL            0x08U
#define UVC_ASYNCHRONOUS_CONTROL          0x10U

typedef struct  {
    /**
     * This function is called when the Host wants to read video data from the specific stream.
     * Applications should send video data via the assigned stream immediately.
     * */
    UINT (*encode_start)(UINT Index);
    /**
     * This function is called when the Host wants to stop reading video data from the specific stream.
     * Applications should not send video data with assigned stream afterwards.
     * */
    UINT (*encode_stop)(UINT Index);
    /**
     * This function is called when the Host wants to read still capture data from the specific stream.
     * Applications should send still capture data via the assigned stream when available.
     * */
    UINT (*still_capture_start)(UINT Index);
    /**
     * This function is called when the Host sends UVC control requests.
     * @param pSetupRequest [Input] USB setup packet for the control request.
     * @param pBuffer       [Output] Data buffer for the control request. Application should fill data if it is a control-in request.
     * @param BufferSize    [Input] Data buffer size.
     * @return 0 This request is handled.
     * @return 0xFFFFFFFF This request is NOT handled. UVC protocol stack would take care of it.
     * @return Others UVC protocol stack would set error code as the value and stall control endpoint.
     * */
    UINT32 (*request_handler)(const USB_SETUP_REQUEST_S *pSetupRequest, UINT8 *pBuffer, UINT32 BufferSize);
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

VOID    uxd_uvc_thread(ULONG arg);
VOID    uxd_uvc_event_thread(ULONG arg);
UINT    uxd_uvc_initialize(const UX_SLAVE_CLASS_COMMAND *command);
UINT    uxd_uvc_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT    uxd_uvc_reset(const UX_SLAVE_CLASS_COMMAND *command);
UINT    uxd_uvc_activate(const UX_SLAVE_CLASS_COMMAND *command);
UINT    uxd_uvc_deactivate(const UX_SLAVE_CLASS_COMMAND *command);
UINT    uxd_uvc_control_request(UX_SLAVE_CLASS_COMMAND *command);
UINT    uxd_uvc_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    uxd_uvc_still_probe_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    uxd_uvc_commit_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    uxd_uvc_still_commit_control(const UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq);
UINT    uxd_uvc_multi_payload_send(UINT32 StreamID, UINT8 *pBuffer, UINT32 Size, UINT32 Timeout, UINT32 NoCopy);
UINT32  uxd_uvc_max_xfer_length_get(UINT32 vs_id);
UX_SLAVE_TRANSFER* uxd_uvc_xfer_request_allocate(UINT32 vs_id);
UINT    uxd_uvc_xfer_request_free(const UX_SLAVE_TRANSFER *transfer_request);
UINT    uxd_uvc_iso_transfer_fill(UX_SLAVE_TRANSFER *                     transfer_request,
                                  VOID *                     iso_req_head,
                                  ULONG                      iso_req_num,
                                  ULONG                      timeout,
                                  ux_slave_transfer_request_completion_fn compl_fn);
UINT    uxd_uvc_iso_transfer_submit(UX_SLAVE_TRANSFER *transfer_request);
UINT    uxd_uvc_status_get(UX_SLAVE_CLASS_UVC *ux_uvc_class, UX_SLAVE_CLASS_UVC_STATUS *uvc_status);
UINT    uxd_uvc_status_set(UX_SLAVE_CLASS_UVC * ux_uvc_class, const UX_SLAVE_CLASS_UVC_STATUS *uvc_status);

void    uxd_uvc_probe_control_config(UINT32 stream_id, UCHAR *data_pointer, ULONG request_length);
void    uxd_uvc_still_probe_config(UCHAR *data_pointer, ULONG request_length);
uvcd_probe_control_t       *uxd_uvc_probe_control_get(UINT32 stream_id);
uvcd_still_probe_control_t *uxd_uvc_still_probe_control_get(void);
void    uxd_uvc_probe_control_copy(const uvcd_probe_control_t *control, UINT8 *out);
void    uxd_uvc_probe_control_decode(UINT8 *data, uvcd_probe_control_t *control);
void    uvcd_decode_still_probe_control(UINT8 *data, uvcd_still_probe_control_t *control);
void    uxd_uvc_ctrl_null_packet_send(UX_SLAVE_TRANSFER *transfer);

void    uxd_uvc_encode_start_notify(UINT idx);
void    uxd_uvc_encode_stop_notify(UINT idx);
void    uxd_uvc_stillcap_start_notify(UINT idx);
UINT32  uxd_uvc_ctrl_request_notify(const USB_SETUP_REQUEST_S *setup_request, UINT8 *buffer, UINT32 buffer_size);

// utility functions exposed
void    uxd_uvc_controll_endpoint_stall(void);
void    uxd_uvc_request_error_code_set(UINT32 value);

UINT    uxd_uvc_callback_register(const uvcd_event_callback_t *pCallBacks);
UINT    uxd_uvc_host_config_get(UINT StreamID, uvcd_host_config_t *pConfig);

UINT    uxd_uvc_probe_max_xfer_size_get(UINT StreamID, UINT *pSize);
void    uxd_uvc_probe_max_xfer_size_set(UINT StreamID, UINT Size);

UX_SLAVE_CLASS_UVC* uxd_uvc_voidp_to_uvcp(const VOID *pVoid);
UX_SLAVE_INTERFACE* uxd_uvc_voidp_to_intfp(const VOID *pVoid);
UX_SLAVE_CLASS_UVC *uxd_uvc_context_get(void);
UX_SLAVE_CLASS     *uxd_uvc_class_get(void);
void                uxd_uvc_context_set(UX_SLAVE_CLASS_UVC *UvcdContext);
void                uxd_uvc_class_set(UX_SLAVE_CLASS *UxClass);

#endif

