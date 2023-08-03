/**
 *  @file ux_device_class_uvc_thread.c
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
 *  @details thread functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * Main thread for sending status through interrupt endpoint
 * It is not used now since no one will set status report.
 * */
VOID  uxd_uvc_thread(ULONG arg)
{
    UX_SLAVE_CLASS *ux_class;
    UINT status;
    UX_SLAVE_CLASS_UVC *ux_uvc_class;
    UX_SLAVE_TRANSFER *transfer_request_in;
    UX_SLAVE_CLASS_UVC_STATUS uvc_status;
    UCHAR *buffer;

    (VOID)arg;

    ux_class = uxd_uvc_class_get();
    ux_uvc_class   = uxd_uvc_context_get();

    /* Allocate the status round robin buffer.  */
    _ux_utility_memory_set(uvcd_status_memory, 0, sizeof(uvcd_status_memory));
    ux_uvc_class->status_array = uvcd_status_memory;

    /* Allocate the head\tail and end of the round robin buffer.  */
    ux_uvc_class->status_array_head = ux_uvc_class->status_array;
    ux_uvc_class->status_array_tail = ux_uvc_class->status_array;
    ux_uvc_class->status_array_end  = &ux_uvc_class->status_array[UX_DEVICE_CLASS_UVC_MAX_STATUS_QUEUE - 1U];

    /* This thread runs forever but can be suspended or resumed.  */
    for (;;) {
        const UX_SLAVE_DEVICE *device;
        device = &_ux_system_slave->ux_system_slave_device;

        /* As long as the device is in the CONFIGURED state.  */
        while (device->ux_slave_device_state == (ULONG)UX_DEVICE_CONFIGURED) {

            // interrupt endpoint
            transfer_request_in = &ux_uvc_class->vc_interface->ux_slave_interface_first_endpoint->ux_slave_endpoint_transfer_request;

            // wait for control request form ISR handler
            status = _ux_utility_semaphore_get(&ux_uvc_class->class_thread_semaphore, 0xffffffffU); //UX_WAIT_FOREVER

            if (status != 0U) {
                _ux_utility_print_uint5("_ux_device_class_uvc_thread(): wait semaphore failed, 0x%X.", status, 0, 0, 0, 0);
                break;
            }

            /* Check if we have an status to report.  */
            status = uxd_uvc_status_get(ux_uvc_class, &uvc_status);

            /* We may have an status to report on the interrupt pipe.  */
            if (status == 0U) {
                ULONG event_length = 0;
                UINT8 status_type  = uvc_status.status_type;

                _ux_utility_print_uint5("status_type  = 0x%x", uvc_status.status_type, 0, 0, 0, 0);
                _ux_utility_print_uint5("originator   = 0x%x", uvc_status.originator, 0, 0, 0, 0);
                _ux_utility_print_uint5("event        = 0x%x", uvc_status.event, 0, 0, 0, 0);
                _ux_utility_print_uint5("selector     = 0x%x", uvc_status.selector, 0, 0, 0, 0);
                _ux_utility_print_uint5("attribut     = 0x%x", uvc_status.attribute, 0, 0, 0, 0);
                _ux_utility_print_uint5("value        = 0x%x", uvc_status.value, 0, 0, 0, 0);
                _ux_utility_print_uint5("value_length = 0x%x", uvc_status.value_length, 0, 0, 0, 0);

                /* Prepare the event data payload from the pima event structure.  Get a pointer to the buffer area.  */
                buffer = transfer_request_in->ux_slave_transfer_request_data_pointer;

                buffer[UX_DEVICE_CLASS_UVC_SI_STATUS_TYPE] = status_type;
                buffer[UX_DEVICE_CLASS_UVC_SI_ORIGINATOR]  = uvc_status.originator;

                switch (status_type) {
                case UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL:
                    buffer[UX_DEVICE_CLASS_UVC_SI_VC_EVENT]     = uvc_status.event;
                    buffer[UX_DEVICE_CLASS_UVC_SI_VC_SELECTOR]  = uvc_status.selector;
                    buffer[UX_DEVICE_CLASS_UVC_SI_VC_ATTRIBUTE] = uvc_status.attribute;
                    _ux_utility_long_put(&buffer[UX_DEVICE_CLASS_UVC_SI_VC_VALUE], uvc_status.value);
                    event_length = (ULONG)UX_DEVICE_CLASS_UVC_SI_VC_VALUE + (ULONG)uvc_status.value_length;
                    break;
                case UX_DEVICE_CLASS_UVC_SI_VIDEO_STREAM:
                    buffer[UX_DEVICE_CLASS_UVC_SI_VS_EVENT] = uvc_status.event;
                    _ux_utility_long_put(&buffer[UX_DEVICE_CLASS_UVC_SI_VS_VALUE], uvc_status.value);
                    event_length = (ULONG)UX_DEVICE_CLASS_UVC_SI_VS_VALUE + (ULONG)uvc_status.value_length;
                    break;
                default:
                    _ux_utility_print_uint5("_ux_device_class_uvc_thread(): Unknown status type 0x%x.", status_type, 0, 0, 0, 0);
                    break;
                }

                /* Send the request to the device controller.  */
                status = _ux_device_stack_transfer_request(transfer_request_in, event_length, 0);

                if (status != 0U) {
                    _ux_utility_print_uint5("_ux_device_class_uvc_thread(): Interrupt transfer error 0x%x", status, 0, 0, 0, 0);
                }
            }
        }

        /* We need to suspend ourselves. We will be resumed by the
           device enumeration module.  */
        if (_ux_utility_thread_suspend(&ux_class->ux_slave_class_thread) != 0U) {
            // ignore error
        }
    }
}

/**
 * Event thread for monitoring clear stall event to stop encode.
 * It is not used now since no one will set event.
 * */
VOID  uxd_uvc_event_thread(ULONG arg)
{
    UINT status;
    UX_SLAVE_CLASS_UVC *ux_uvc_class;

    (VOID)arg;

    /* This thread runs forever but can be suspended or resumed.  */
    for (;;) {
        /* Get the storage instance from this class container.  */
        ux_uvc_class = uxd_uvc_context_get();

        // wait for event callbacks form ISR handler
        status = _ux_utility_semaphore_get(&ux_uvc_class->event_thread_semaphore, 0xffffffffU); //UX_WAIT_FOREVER

        if (status != 0U) {
            _ux_utility_print_uint5("_uvcd_event_thread(): wait semaphore failed, 0x%X.", status, 0, 0, 0, 0);
            break;
        }

        // check the event
        _ux_utility_print_uint5("_uvcd_event_thread(): EventID = 0x%X", ux_uvc_class->event_id, 0, 0, 0, 0);
        if (UDC_UVC_EVENT_CLEAR_STALL == ux_uvc_class->event_id) {
            // check which stream id deserves it
            UINT32 vs_id                 = 0;
            UINT32 found                 = 0;
            const UVCD_VS_INSTANCE *uvc_vs_cur = ux_uvc_class->first_vs_instance;
            UINT32 ep_addr               = 0;
            while (uvc_vs_cur != NULL) {
                ep_addr = uvc_vs_cur->ux_interface->ux_slave_interface_first_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
                if (ep_addr == ux_uvc_class->event_endpoint_id) {
                    vs_id = uvc_vs_cur->stream_id;
                    found = 1;
                    break;
                }
                uvc_vs_cur = uvc_vs_cur->next;
            }

            if (found == 0U) {
                _ux_utility_print_uint5("_uvcd_event_thread(): no match video stream instance for endpoint 0x%x", ux_uvc_class->event_endpoint_id, 0, 0, 0, 0);
            } else {
                uxd_uvc_encode_stop_notify(vs_id);
            }
        }
    }
}
/** @} */
