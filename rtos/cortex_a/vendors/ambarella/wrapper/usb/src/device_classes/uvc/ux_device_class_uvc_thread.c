/**
 *  @file ux_device_class_uvc_thread.c
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
 *  @details thread functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

// Used for status interrupt pipe
VOID  _ux_device_class_uvc_thread(ULONG uvc_class)
{
    UX_SLAVE_CLASS *pTmpClass;
    UINT status;
    UX_SLAVE_CLASS_UVC *pUVC;
    UX_SLAVE_TRANSFER *transfer_request_in;
    UX_SLAVE_CLASS_UVC_STATUS uvc_status;
    UCHAR *buffer;

    (VOID)uvc_class;

    pTmpClass = uvcd_get_class();
    pUVC   = uvcd_get_context();

    /* Allocate the status round robin buffer.  */
    _ux_utility_memory_set(uvcd_status_memory, 0, sizeof(uvcd_status_memory));
    pUVC->uvcd_status_array = uvcd_status_memory;

    /* Allocate the head\tail and end of the round robin buffer.  */
    pUVC->uvcd_status_array_head = pUVC->uvcd_status_array;
    pUVC->uvcd_status_array_tail = pUVC->uvcd_status_array;
    pUVC->uvcd_status_array_end  = &pUVC->uvcd_status_array[UX_DEVICE_CLASS_UVC_MAX_STATUS_QUEUE - 1U];

    /* This thread runs forever but can be suspended or resumed.  */
    for (;;) {
        const UX_SLAVE_DEVICE *device;
        device = &_ux_system_slave->ux_system_slave_device;

        /* As long as the device is in the CONFIGURED state.  */
        while (device->ux_slave_device_state == (ULONG)UX_DEVICE_CONFIGURED) {

            // interrupt endpoint
            transfer_request_in = &pUVC->vc_interface->ux_slave_interface_first_endpoint->ux_slave_endpoint_transfer_request;

            // wait for control request form ISR handler
            status = _ux_utility_semaphore_get(&pUVC->class_thread_semaphore, 0xffffffffU); //UX_WAIT_FOREVER

            if (status != 0U) {
                _ux_utility_print_uint5("_ux_device_class_uvc_thread(): wait semaphore failed, 0x%X.", status, 0, 0, 0, 0);
                break;
            }

            /* Check if we have an status to report.  */
            status = _ux_device_class_uvc_status_get(pUVC, &uvc_status);

            /* We may have an status to report on the interrupt pipe.  */
            if (status == 0U) {
                ULONG event_length = 0;
                UINT8 status_type  = uvc_status.uvcd_status_type;

                _ux_utility_print_uint5("status_type  = 0x%x", uvc_status.uvcd_status_type, 0, 0, 0, 0);
                _ux_utility_print_uint5("originator   = 0x%x", uvc_status.uvcd_originator, 0, 0, 0, 0);
                _ux_utility_print_uint5("event        = 0x%x", uvc_status.uvcd_event, 0, 0, 0, 0);
                _ux_utility_print_uint5("selector     = 0x%x", uvc_status.uvcd_selector, 0, 0, 0, 0);
                _ux_utility_print_uint5("attribut     = 0x%x", uvc_status.uvcd_attribute, 0, 0, 0, 0);
                _ux_utility_print_uint5("value        = 0x%x", uvc_status.uvcd_value, 0, 0, 0, 0);
                _ux_utility_print_uint5("value_length = 0x%x", uvc_status.uvcd_value_length, 0, 0, 0, 0);

                /* Prepare the event data payload from the pima event structure.  Get a pointer to the buffer area.  */
                buffer = transfer_request_in->ux_slave_transfer_request_data_pointer;

                buffer[UX_DEVICE_CLASS_UVC_SI_STATUS_TYPE] = status_type;
                buffer[UX_DEVICE_CLASS_UVC_SI_ORIGINATOR]  = uvc_status.uvcd_originator;

                switch (status_type) {
                case UX_DEVICE_CLASS_UVC_SI_VIDEO_CONTROL:
                    buffer[UX_DEVICE_CLASS_UVC_SI_VC_EVENT]     = uvc_status.uvcd_event;
                    buffer[UX_DEVICE_CLASS_UVC_SI_VC_SELECTOR]  = uvc_status.uvcd_selector;
                    buffer[UX_DEVICE_CLASS_UVC_SI_VC_ATTRIBUTE] = uvc_status.uvcd_attribute;
                    _ux_utility_long_put(&buffer[UX_DEVICE_CLASS_UVC_SI_VC_VALUE], uvc_status.uvcd_value);
                    event_length = (ULONG)UX_DEVICE_CLASS_UVC_SI_VC_VALUE + (ULONG)uvc_status.uvcd_value_length;
                    break;
                case UX_DEVICE_CLASS_UVC_SI_VIDEO_STREAM:
                    buffer[UX_DEVICE_CLASS_UVC_SI_VS_EVENT] = uvc_status.uvcd_event;
                    _ux_utility_long_put(&buffer[UX_DEVICE_CLASS_UVC_SI_VS_VALUE], uvc_status.uvcd_value);
                    event_length = (ULONG)UX_DEVICE_CLASS_UVC_SI_VS_VALUE + (ULONG)uvc_status.uvcd_value_length;
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
        if (_ux_utility_thread_suspend(&pTmpClass->ux_slave_class_thread) != 0U) {
            // ignore error
        }
    }
}

VOID  _uvcd_event_thread(ULONG uvc_class)
{
    UINT status;
    UX_SLAVE_CLASS_UVC *pUVC;

    (VOID)uvc_class;

    /* This thread runs forever but can be suspended or resumed.  */
    for (;;) {
        /* Get the storage instance from this class container.  */
        pUVC = uvcd_get_context();

        // wait for event callbacks form ISR handler
        status = _ux_utility_semaphore_get(&pUVC->event_thread_semaphore, 0xffffffffU); //UX_WAIT_FOREVER

        if (status != 0U) {
            _ux_utility_print_uint5("_uvcd_event_thread(): wait semaphore failed, 0x%X.", status, 0, 0, 0, 0);
            break;
        }

        // check the event
        _ux_utility_print_uint5("_uvcd_event_thread(): EventID = 0x%X", pUVC->event_id, 0, 0, 0, 0);
        if (UDC_UVC_EVENT_CLEAR_STALL == pUVC->event_id) {
            // check which stream id deserves it
            UINT32 vs_id                 = 0;
            UINT32 found                 = 0;
            const UVCD_VS_INSTANCE *uvc_vs_cur = pUVC->first_vs_instance;
            UINT32 ep_addr               = 0;
            while (uvc_vs_cur != NULL) {
                ep_addr = uvc_vs_cur->vs->ux_slave_interface_first_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
                if (ep_addr == pUVC->event_endpoint_id) {
                    vs_id = uvc_vs_cur->vs_id;
                    found = 1;
                    break;
                }
                uvc_vs_cur = uvc_vs_cur->next_vs;
            }

            if (found == 0U) {
                _ux_utility_print_uint5("_uvcd_event_thread(): no match video stream instance for endpoint 0x%x", pUVC->event_endpoint_id, 0, 0, 0, 0);
            } else {
                uvcd_notify_encode_stop(vs_id);
            }
        }
    }
}

