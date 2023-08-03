/**
 *  @file ux_device_class_uvc_reset.c
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
 *  @details reset/release functions of the uvc device class
 */

#define UX_SOURCE_CODE

/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"

/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * Called by USBX to de-initialize class instance. Resources would be released here.
*/
UINT  uxd_uvc_reset(const UX_SLAVE_CLASS_COMMAND *command)
{
    UINT status;
    UX_SLAVE_CLASS *ux_class;
    UX_SLAVE_CLASS_UVC *ux_uvc_class;
    UINT uret = UX_SUCCESS;

    ux_class     = command->ux_slave_class_command_class_ptr;
    ux_uvc_class = uxd_uvc_voidp_to_uvcp(ux_class->ux_slave_class_instance);

    // delete UVC thread
    if (ux_class->ux_slave_class_thread.tx_thread_id != 0U) {
        status = _ux_utility_thread_delete(&ux_class->ux_slave_class_thread);
        if (status != 0U) {
            _ux_utility_print_uint5("_ux_device_class_uvc_reset(): Can't delete UVC thread, 0x%X", status, 0, 0, 0, 0);
            uret = status;
        }
    }

    // delete a semaphore of the UVC thread.
    if (ux_uvc_class->class_thread_semaphore.tx_semaphore_id != 0U) {
        status = _ux_utility_semaphore_delete(&ux_uvc_class->class_thread_semaphore);
        if (status != 0U) {
            _ux_utility_print_uint5("_ux_device_class_uvc_reset(): delete UVC semaphore fail (0x%X), 0x%X", ux_uvc_class->class_thread_semaphore.tx_semaphore_id, status, 0, 0, 0);
            uret = status;
        }
    }

    // delete UVC event thread
    if (ux_uvc_class->event_thread.tx_thread_id != 0U) {
        status = _ux_utility_thread_delete(&ux_uvc_class->event_thread);
        if (status != 0U) {
            _ux_utility_print_uint5("_ux_device_class_uvc_reset(): Can't delete UVC event thread, 0x%X", status, 0, 0, 0, 0);
            uret = status;
        }
    }

    // delete a semaphore of the UVC event thread.
    if (ux_uvc_class->event_thread_semaphore.tx_semaphore_id != 0U) {
        status = _ux_utility_semaphore_delete(&ux_uvc_class->event_thread_semaphore);
        if (status != 0U) {
            _ux_utility_print_uint5("_ux_device_class_uvc_reset(): delete UVC event semaphore fail (0x%X), 0x%X", ux_uvc_class->event_thread_semaphore.tx_semaphore_id, status, 0, 0, 0);
            uret = status;
        }
    }

    // delete a mutex of the UVC request allocation/free.
    if (ux_uvc_class->request_mutex.tx_mutex_id != 0U) {
        status = _ux_utility_mutex_delete(&ux_uvc_class->request_mutex);
        if (status != 0U) {
            _ux_utility_print_uint5("_ux_device_class_uvc_reset(): delete UVC request mutex fail (0x%X), 0x%X", ux_uvc_class->request_mutex.tx_mutex_id, status, 0, 0, 0);
            uret = status;
        }
    }

    uxd_uvc_context_set(NULL);
    uxd_uvc_class_set(NULL);

    return (uret);
}
/** @} */
