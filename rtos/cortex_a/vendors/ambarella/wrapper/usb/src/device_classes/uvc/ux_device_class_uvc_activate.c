/**
 *  @file ux_device_class_uvc_activate.c
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
 *  @details active functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

UX_SLAVE_CLASS_UVC* uvc_class_void_2_uvc(const VOID *pVoid)
{
    const VOID *pTmpVoid = pVoid;
    UX_SLAVE_CLASS_UVC *pTmpUVC;

    _ux_utility_memory_copy(&pTmpUVC, &pTmpVoid, sizeof(VOID*));

    return pTmpUVC;
}

UX_SLAVE_INTERFACE* uvc_class_void_2_interface(const VOID *pVoid)
{
    const VOID *pTmpVoid = pVoid;
    UX_SLAVE_INTERFACE *pTmpInterface;

    _ux_utility_memory_copy(&pTmpInterface, &pTmpVoid, sizeof(VOID*));

    return pTmpInterface;
}

UINT  _ux_device_class_uvc_activate(const UX_SLAVE_CLASS_COMMAND *command)
{
    UINT status;
    UX_SLAVE_INTERFACE *pInterface;
    UX_SLAVE_CLASS_UVC *pUVC;
    UX_SLAVE_CLASS *pTmpClass;


    /* Get the class container.  */
    pTmpClass = command->ux_slave_class_command_class_ptr;

    /* Get the class instance in the container.  */
    pUVC = uvc_class_void_2_uvc(pTmpClass->ux_slave_class_instance);

    /* Get the interface that owns this instance.  */
    pInterface = uvc_class_void_2_interface(command->ux_slave_class_command_interface);

    /* Store the class instance into the interface.  */
    pInterface->ux_slave_interface_class_instance = pTmpClass->ux_slave_class_instance;

    /* Now the opposite, store the interface in the class instance.  */
    pUVC->ux_slave_class_uvc_interface = pInterface;

    /* Resume thread.  */
    /* To avoid that slave thread will be resumed twice, we have to check thread state.*/
    if (pTmpClass->ux_slave_class_thread.tx_thread_state == (UINT)TX_SUSPENDED) {
    	TX_THREAD *thread = &pTmpClass->ux_slave_class_thread;
    	status = _ux_utility_thread_resume(thread);
    } else {
        status = UX_SUCCESS;
    }

    if (pInterface->ux_slave_interface_descriptor.bInterfaceNumber == 0U) {
        /* Video control interface */
        pUVC->vc_interface = pInterface;
    } else if (pUVC->vs_info.vs_num >= UVCD_MAX_VS_INSTANCE) {
        _ux_utility_print("_ux_device_class_uvc_activate(): reach max vs number 5");
        status = UX_MEMORY_ARRAY_FULL;
    } else {
        /* Video stream interface */
        /* Create vs instance*/
        UVCD_VS_INSTANCE *uvc_vs;
        //UINT32 cur_interface_number = pInterface->ux_slave_interface_descriptor.bInterfaceNumber;

        uvc_vs = &uvcd_vs_memory[pUVC->vs_info.vs_num];

        uvc_vs->vs    = pInterface;
        uvc_vs->vs_id = pUVC->vs_info.vs_num;
        pUVC->vs_info.vs_num++;
        uvc_vs->next_vs = NULL;
        if (pUVC->first_vs_instance == NULL) {
            pUVC->first_vs_instance = uvc_vs;
        } else {
            UVCD_VS_INSTANCE *cur_instance = pUVC->first_vs_instance;
            while (cur_instance->next_vs != NULL) {
                cur_instance = cur_instance->next_vs;
            }
            cur_instance->next_vs = uvc_vs;
        }

        //_ux_utility_print_uint5("_ux_device_class_uvc_activate(): [%u] Created video stream instance 0x%x ", cur_interface_number, uvc_vs, 0, 0, 0);
        //_ux_utility_print_uint5("		current vs number = %u", pUVC->vs_info.vs_num, 0, 0, 0, 0);
    }

    /* If trace is enabled, insert this event into the trace buffer.  */
    //UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_CLASS_UVC_ACTIVATE, 0, 0, 0, 0, UX_TRACE_DEVICE_CLASS_EVENTS, 0, 0)

    /* If trace is enabled, register this object.  */
    //UX_TRACE_OBJECT_REGISTER(UX_TRACE_DEVICE_OBJECT_TYPE_INTERFACE, 0, 0, 0, 0)

    /* Return completion status.  */
    return (status);
}

