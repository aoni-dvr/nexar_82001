/**
 *  @file ux_device_class_uvc_status_set.c
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
 *  @details status set functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

UINT  _ux_device_class_uvc_status_set(UX_SLAVE_CLASS_UVC *       uvc,
                                     const  UX_SLAVE_CLASS_UVC_STATUS *uvc_status)
{
    UX_SLAVE_CLASS_UVC_STATUS *current_uvc_status;
    UX_SLAVE_CLASS_UVC_STATUS *next_uvc_status;
    const UX_SLAVE_DEVICE *device;
    UINT uRet = UX_SUCCESS;

    /* If trace is enabled, insert this event into the trace buffer.  */
    //UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_CLASS_UVC_STATUS_SET, uvc, uvc_status, 0, 0, UX_TRACE_DEVICE_CLASS_EVENTS, 0, 0)

    /* Get the pointer to the device.  */
    device = &_ux_system_slave->ux_system_slave_device;

    /* Check the device state.  */
    if (device->ux_slave_device_state != (ULONG)UX_DEVICE_CONFIGURED) {
        /* If trace is enabled, insert this event into the trace buffer.  */
        //UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_DEVICE_HANDLE_UNKNOWN, device, 0, 0, UX_TRACE_ERRORS, 0, 0)
        uRet = UX_DEVICE_HANDLE_UNKNOWN;
    } else {
        /* Current position of the head.  */
        current_uvc_status = uvc->uvcd_status_array_head;

        /* If the pointer is NULL, the round robin buffer has not been activated.  */
        if (current_uvc_status == UX_NULL) {
            uRet = (UX_ERROR);
        } else {
            //_ux_utility_print_uint5("_ux_device_class_uvc_status_set(): Status addr = 0x%x", current_uvc_status, 0, 0, 0, 0);

            /* fill in the status structure from the user.  */
            current_uvc_status->uvcd_status_type  = uvc_status->uvcd_status_type;
            current_uvc_status->uvcd_originator   = uvc_status->uvcd_originator;
            current_uvc_status->uvcd_event        = uvc_status->uvcd_event;
            current_uvc_status->uvcd_selector     = uvc_status->uvcd_selector;
            current_uvc_status->uvcd_attribute    = uvc_status->uvcd_attribute;
            current_uvc_status->uvcd_value        = uvc_status->uvcd_value;
            current_uvc_status->uvcd_value_length = uvc_status->uvcd_value_length;


            /* Calculate the next position.  */
            if (&current_uvc_status[1] == uvc->uvcd_status_array_end) {
                /* We are at the end, go back to the beginning.  */
                next_uvc_status = uvc->uvcd_status_array;
            } else {
                /* We are not at the end, increment the head position.  */
                next_uvc_status = &current_uvc_status[1];
            }

            /* Any place left for this status ? */
            if (next_uvc_status == uvc->uvcd_status_array_tail) {
                /* array is full */
                _ux_utility_print("_ux_device_class_uvc_status_set(): status array is full");
                uRet = (UX_ERROR);
            } else {
                /* Update the head.  */
                uvc->uvcd_status_array_head = next_uvc_status;

                /* Set a semaphore to wake up the interrupt thread.  */
                uRet = _ux_utility_semaphore_put(&uvc->class_thread_semaphore);
            }
        }
    }

    /* Return event status to the user.  */
    return uRet;
}

