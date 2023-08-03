/**
 *  @file ux_device_class_uvc_entry.c
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
 *  @details entry functions of the uvc device class
 */

/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

UINT  _ux_device_class_uvc_entry(UX_SLAVE_CLASS_COMMAND *command)
{
    UINT status;

    switch (command->ux_slave_class_command_request) {
    case UX_SLAVE_CLASS_COMMAND_INITIALIZE:
        status = _ux_device_class_uvc_initialize(command);
        break;

    case UX_SLAVE_CLASS_COMMAND_RESET:
        status = _ux_device_class_uvc_reset(command);
        break;

    case UX_SLAVE_CLASS_COMMAND_QUERY:
        if (command->ux_slave_class_command_class == (UINT)UX_SLAVE_CLASS_UVC_CLASS) {
            status = UX_SUCCESS;
        } else {
            _ux_utility_print_uint5("_ux_device_class_uvc_entry(): class(%u) code doesn't match(%u).", command->ux_slave_class_command_class, UX_SLAVE_CLASS_UVC_CLASS, 0, 0, 0);
            status = UX_NO_CLASS_MATCH;
        }
        break;

    case UX_SLAVE_CLASS_COMMAND_ACTIVATE:
    	status = _ux_device_class_uvc_activate(command);

        // To pass usbcv
        {
            uvcd_pu_wbt_auto_t wbt_auto;
            if (uvcd_pu_get_wbt_auto(&wbt_auto) == 0U) {
                wbt_auto.current_value = 1;
                if (uvcd_pu_set_wbt_auto(&wbt_auto) != 0U) {
                    // ignore error
                }
            }
        }
        break;

    case UX_SLAVE_CLASS_COMMAND_DEACTIVATE:
        status = _ux_device_class_uvc_deactivate(command);
        break;

    case UX_SLAVE_CLASS_COMMAND_CHANGE:
        status = UX_SUCCESS;
        break;

    case UX_SLAVE_CLASS_COMMAND_REQUEST:
        status = _ux_device_class_uvc_control_request(command);
        break;

    default:
        /* If trace is enabled, insert this event into the trace buffer.  */
        //UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_FUNCTION_NOT_SUPPORTED, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)
        status = UX_FUNCTION_NOT_SUPPORTED;
        break;
    }
    return status;
}

