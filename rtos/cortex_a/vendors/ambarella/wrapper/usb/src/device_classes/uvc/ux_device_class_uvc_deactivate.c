/**
 *  @file ux_device_class_uvc_deactivate.c
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
 *  @details deactive functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

UINT  _ux_device_class_uvc_deactivate(const UX_SLAVE_CLASS_COMMAND *command)
{
    const UX_SLAVE_INTERFACE *pInterface;
    UX_SLAVE_CLASS_UVC *pUVC;
    const UX_SLAVE_CLASS *pTmpClass;
    UVCD_VS_INSTANCE *uvc_vs_pre = NULL, *uvc_vs_cur;
    UINT32 vs_loop = 0;

    /* Get the class container.  */
    pTmpClass = command->ux_slave_class_command_class_ptr;

    /* Get the class instance in the container.  */
    pUVC = uvc_class_void_2_uvc(pTmpClass->ux_slave_class_instance);

    /* Get the interface that owns this instance.  */
    pInterface = uvc_class_void_2_interface(command->ux_slave_class_command_interface);

    _ux_utility_print_uint5("_ux_device_class_uvc_deactivate(): interface nubmer = %d", pInterface->ux_slave_interface_descriptor.bInterfaceNumber, 0, 0, 0, 0);

    /* Remove video stream instance */
    if (pInterface->ux_slave_interface_descriptor.bInterfaceNumber != 0U) {
        // Remove video stream instance by interface number
        UINT32 cur_interface_number = pInterface->ux_slave_interface_descriptor.bInterfaceNumber;
        UINT32 found                = 0;
        uvc_vs_cur = pUVC->first_vs_instance;
        while (uvc_vs_cur != NULL) {
            if (uvc_vs_cur->vs->ux_slave_interface_descriptor.bInterfaceNumber == cur_interface_number) {
                if (vs_loop == 0U) {
                    pUVC->first_vs_instance = uvc_vs_cur->next_vs;
                } else {
                    uvc_vs_pre->next_vs = uvc_vs_cur->next_vs;
                }
                _ux_utility_memory_free(uvc_vs_cur);
                pUVC->vs_info.vs_num--;
                found = 1;
                break;
            }
            vs_loop++;
            uvc_vs_pre = uvc_vs_cur;
            uvc_vs_cur = uvc_vs_cur->next_vs;
        }

        if (found == 0U) {
            _ux_utility_print_uint5("_ux_device_class_uvc_deactivate(): Can't remove video stream instance with number = %d", cur_interface_number, 0, 0, 0, 0);
        } else {
            //_ux_utility_print_uint5("_ux_device_class_uvc_deactivate(): [%d] Removed video stream instance = 0x%x", cur_interface_number, uvc_vs_cur, 0, 0, 0);
            //_ux_utility_print_uint5("		 Residual vs number = %d", pUVC->vs_info.vs_num, 0, 0, 0, 0);
        }
    }

    /* If trace is enabled, insert this event into the trace buffer.  */
    //UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_CLASS_UVC_ACTIVATE, 0, 0, 0, 0, UX_TRACE_DEVICE_CLASS_EVENTS, 0, 0)

    /* If trace is enabled, register this object.  */
    //UX_TRACE_OBJECT_REGISTER(UX_TRACE_DEVICE_OBJECT_TYPE_INTERFACE, 0, 0, 0, 0)

    /* Return completion status.  */
    return (UX_SUCCESS);
}

