/**
 *  @file ux_device_class_uvc_activate.c
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
 *  @details active functions of the uvc device class
 */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_device_class_uvc.h"

/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * convert void pointer to USBX UVC Slave Class pointer.
 * It is for passing MISRA-C
*/
UX_SLAVE_CLASS_UVC* uxd_uvc_voidp_to_uvcp(const VOID *pVoid)
{
    const VOID *pTmpVoid = pVoid;
    UX_SLAVE_CLASS_UVC *pTmpUVC;

    _ux_utility_memory_copy(&pTmpUVC, &pTmpVoid, sizeof(VOID*));

    return pTmpUVC;
}

/**
 * convert void pointer to USBX Slave Interface pointer.
 * It is for passing MISRA-C
*/
UX_SLAVE_INTERFACE* uxd_uvc_voidp_to_intfp(const VOID *pVoid)
{
    const VOID *pTmpVoid = pVoid;
    UX_SLAVE_INTERFACE *pTmpInterface;

    _ux_utility_memory_copy(&pTmpInterface, &pTmpVoid, sizeof(VOID*));

    return pTmpInterface;
}
/**
 * Called by USBX to activate class instance
*/
UINT  uxd_uvc_activate(const UX_SLAVE_CLASS_COMMAND *command)
{
    UINT status;
    UX_SLAVE_INTERFACE *ux_intf;
    UX_SLAVE_CLASS_UVC *uvcd_class;
    UX_SLAVE_CLASS *ux_class;

    /* Get the class container.  */
    ux_class = command->ux_slave_class_command_class_ptr;

    /* Get the class instance in the container.  */
    uvcd_class = uxd_uvc_voidp_to_uvcp(ux_class->ux_slave_class_instance);

    /* Get the interface that owns this instance.  */
    ux_intf = uxd_uvc_voidp_to_intfp(command->ux_slave_class_command_interface);

    /* Store the class instance into the interface.  */
    ux_intf->ux_slave_interface_class_instance = ux_class->ux_slave_class_instance;

    /* Now the opposite, store the interface in the class instance.  */
    uvcd_class->ux_interface = ux_intf;

    /* Resume thread.  */
    /* To avoid that slave thread will be resumed twice, we have to check thread state.*/
    if (ux_class->ux_slave_class_thread.tx_thread_state == (UINT)TX_SUSPENDED) {
        TX_THREAD *thread = &ux_class->ux_slave_class_thread;
        status = _ux_utility_thread_resume(thread);
    } else {
        status = UX_SUCCESS;
    }

    if (ux_intf->ux_slave_interface_descriptor.bInterfaceNumber == 0U) {
        /* Video control interface */
        uvcd_class->vc_interface = ux_intf;
    } else if (uvcd_class->vs_info.vs_num >= UVCD_MAX_VS_INSTANCE) {
        _ux_utility_print("_ux_device_class_uvc_activate(): reach max vs number 5");
        status = UX_MEMORY_ARRAY_FULL;
    } else {
        /* Video stream interface */
        /* Create vs instance*/
        UVCD_VS_INSTANCE *uvc_vs;
        //UINT32 cur_interface_number = ux_intf->ux_slave_interface_descriptor.bInterfaceNumber;

        uvc_vs = &uvcd_vs_memory[uvcd_class->vs_info.vs_num];

        uvc_vs->ux_interface = ux_intf;
        uvc_vs->stream_id    = uvcd_class->vs_info.vs_num;
        uvcd_class->vs_info.vs_num++;
        uvc_vs->next = NULL;
        if (uvcd_class->first_vs_instance == NULL) {
            uvcd_class->first_vs_instance = uvc_vs;
        } else {
            UVCD_VS_INSTANCE *cur_instance = uvcd_class->first_vs_instance;
            while (cur_instance->next != NULL) {
                cur_instance = cur_instance->next;
            }
            cur_instance->next = uvc_vs;
        }

        //_ux_utility_print_uint5("_ux_device_class_uvc_activate(): [%u] Created video stream instance 0x%x ", cur_interface_number, uvc_vs, 0, 0, 0);
        //_ux_utility_print_uint5("        current vs number = %u", uvcd_class->vs_info.vs_num, 0, 0, 0, 0);
    }

    return (status);
}
/** @} */
