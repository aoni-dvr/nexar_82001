/**
 *  @file ux_device_class_uvc_status_set.c
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
 *  @details status set functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"

/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * Called by application to set status.
*/
UINT  uxd_uvc_status_set(UX_SLAVE_CLASS_UVC *ux_uvc_class,
                         const  UX_SLAVE_CLASS_UVC_STATUS *uvc_status)
{
    UX_SLAVE_CLASS_UVC_STATUS *current_uvc_status;
    UX_SLAVE_CLASS_UVC_STATUS *next_uvc_status;
    const UX_SLAVE_DEVICE *ux_device;
    UINT uret = UX_SUCCESS;

    /* Get the pointer to the ux_device.  */
    ux_device = &_ux_system_slave->ux_system_slave_device;

    /* Check the device state.  */
    if (ux_device->ux_slave_device_state != (ULONG)UX_DEVICE_CONFIGURED) {
        uret = UX_DEVICE_HANDLE_UNKNOWN;
    } else {
        /* Current position of the head.  */
        current_uvc_status = ux_uvc_class->status_array_head;

        /* If the pointer is NULL, the round robin buffer has not been activated.  */
        if (current_uvc_status == UX_NULL) {
            uret = (UX_ERROR);
        } else {

            /* fill in the status structure from the user.  */
            current_uvc_status->status_type  = uvc_status->status_type;
            current_uvc_status->originator   = uvc_status->originator;
            current_uvc_status->event        = uvc_status->event;
            current_uvc_status->selector     = uvc_status->selector;
            current_uvc_status->attribute    = uvc_status->attribute;
            current_uvc_status->value        = uvc_status->value;
            current_uvc_status->value_length = uvc_status->value_length;

            /* Calculate the next position.  */
            if (&current_uvc_status[1] == ux_uvc_class->status_array_end) {
                /* We are at the end, go back to the beginning.  */
                next_uvc_status = ux_uvc_class->status_array;
            } else {
                /* We are not at the end, increment the head position.  */
                next_uvc_status = &current_uvc_status[1];
            }

            /* Any place left for this status ? */
            if (next_uvc_status == ux_uvc_class->status_array_tail) {
                /* array is full */
                _ux_utility_print("uxd_uvc_status_set(): status array is full");
                uret = (UX_ERROR);
            } else {
                /* Update the head.  */
                ux_uvc_class->status_array_head = next_uvc_status;

                /* Set a semaphore to wake up the interrupt thread.  */
                uret = _ux_utility_semaphore_put(&ux_uvc_class->class_thread_semaphore);
            }
        }
    }

    return uret;
}
/** @} */
