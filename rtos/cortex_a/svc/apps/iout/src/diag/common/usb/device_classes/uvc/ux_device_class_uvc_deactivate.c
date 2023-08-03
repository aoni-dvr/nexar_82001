/**
 *  @file ux_device_class_uvc_deactivate.c
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
 *  @details deactive functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"

UINT  uxd_uvc_deactivate(const UX_SLAVE_CLASS_COMMAND *command)
{
    const UX_SLAVE_INTERFACE *ux_intf;
    UX_SLAVE_CLASS_UVC *uvcd_class;
    const UX_SLAVE_CLASS *ux_class;
    UVCD_VS_INSTANCE *uvcd_vs_pre = NULL, *uvcd_vs_cur;
    UINT32 vs_loop = 0;

    /* Get the class container.  */
    ux_class = command->ux_slave_class_command_class_ptr;

    /* Get the class instance in the container.  */
    uvcd_class = uxd_uvc_voidp_to_uvcp(ux_class->ux_slave_class_instance);

    /* Get the interface that owns this instance.  */
    ux_intf = uxd_uvc_voidp_to_intfp(command->ux_slave_class_command_interface);

    _ux_utility_print_uint5("_ux_device_class_uvc_deactivate(): interface nubmer = %d", ux_intf->ux_slave_interface_descriptor.bInterfaceNumber, 0, 0, 0, 0);

    /* Remove video stream instance */
    if (ux_intf->ux_slave_interface_descriptor.bInterfaceNumber != 0U) {
        // Remove video stream instance by interface number
        UINT32 cur_interface_number = ux_intf->ux_slave_interface_descriptor.bInterfaceNumber;
        UINT32 found                = 0;
        uvcd_vs_cur = uvcd_class->first_vs_instance;
        while (uvcd_vs_cur != NULL) {
            if (uvcd_vs_cur->ux_interface->ux_slave_interface_descriptor.bInterfaceNumber == cur_interface_number) {
                if (vs_loop == 0U) {
                    uvcd_class->first_vs_instance = uvcd_vs_cur->next;
                } else {
                    if (uvcd_vs_pre != NULL) {
                        uvcd_vs_pre->next = uvcd_vs_cur->next;
                    }
                }
                _ux_utility_memory_free(uvcd_vs_cur);
                uvcd_class->vs_info.vs_num--;
                found = 1;
                break;
            }
            vs_loop++;
            uvcd_vs_pre = uvcd_vs_cur;
            uvcd_vs_cur = uvcd_vs_cur->next;
        }

        if (found == 0U) {
            _ux_utility_print_uint5("_ux_device_class_uvc_deactivate(): Can't remove video stream instance with number = %d", cur_interface_number, 0, 0, 0, 0);
        } else {
            //_ux_utility_print_uint5("_ux_device_class_uvc_deactivate(): [%d] Removed video stream instance = 0x%x", cur_interface_number, uvcd_vs_cur, 0, 0, 0);
            //_ux_utility_print_uint5("         Residual vs number = %d", uvcd_class->vs_info.vs_num, 0, 0, 0, 0);
        }
    }

    return (UX_SUCCESS);
}

