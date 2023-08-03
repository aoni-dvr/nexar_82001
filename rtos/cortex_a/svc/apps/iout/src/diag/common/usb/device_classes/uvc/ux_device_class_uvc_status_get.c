/**
 *  @file ux_device_class_uvc_status_get.c
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
 *  @details status get functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"

/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * Called by main thread for getting status to report through interrupt endpoint.
*/
UINT  uxd_uvc_status_get(UX_SLAVE_CLASS_UVC *ux_uvc_class,
                         UX_SLAVE_CLASS_UVC_STATUS *uvc_status)
{
    const UX_SLAVE_CLASS_UVC_STATUS *current_uvc_status;
    UINT uret = UX_SUCCESS;

    /* Check if the head and the tail of the status array is the same.  */
    if (ux_uvc_class->status_array_head ==
        ux_uvc_class->status_array_tail) {
        /* No status to report.  */
        _ux_utility_print("uxd_uvc_status_get(): No status");

        uret = (UX_ERROR);
    } else {
        /* There is an status to report, get the current pointer to the status.  */
        current_uvc_status = ux_uvc_class->status_array_tail;

        //_ux_utility_print_uint5("uxd_uvc_status_get(): Status addr = 0x%x", current_uvc_status, 0, 0, 0, 0);

        /* fill in the status structure from the user.  */
        uvc_status->status_type  = current_uvc_status->status_type;
        uvc_status->originator   = current_uvc_status->originator;
        uvc_status->event        = current_uvc_status->event;
        uvc_status->selector     = current_uvc_status->selector;
        uvc_status->attribute    = current_uvc_status->attribute;
        uvc_status->value        = current_uvc_status->value;
        uvc_status->value_length = current_uvc_status->value_length;


        /* Adjust the tail pointer.  Check if we are at the end.  */
        if (&current_uvc_status[1] == ux_uvc_class->status_array_end) {
            /* We are at the end, go back to the beginning.  */
            ux_uvc_class->status_array_tail = ux_uvc_class->status_array;
        } else {
            /* We are not at the end, increment the tail position.  */
            ux_uvc_class->status_array_tail++;
        }
    }

    /* Return event status to the user.  */
    return uret;
}
/** @} */
