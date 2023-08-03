/**
 *  @file ux_device_class_uvc_initialize.c
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
 *  @details init functions of the uvc device class
 */

#define UX_SOURCE_CODE

/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"

static UX_SLAVE_CLASS_UVC *uvcd_context = NULL;
static UX_SLAVE_CLASS     *uvcd_class   = NULL;


/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * Set USBX UVC Slave Class instance
*/
void uxd_uvc_context_set(UX_SLAVE_CLASS_UVC *UvcdContext)
{
    uvcd_context = UvcdContext;
    return;
}
/**
 * Get USBX UVC Slave Class instance
*/
UX_SLAVE_CLASS_UVC *uxd_uvc_context_get(void)
{
    return uvcd_context;
}
/**
 * Set USBX Slave Class instance
*/
void uxd_uvc_class_set(UX_SLAVE_CLASS *UxClass)
{
    uvcd_class = UxClass;
}

/**
 * Get USBX Slave Class instance
*/
UX_SLAVE_CLASS *uxd_uvc_class_get(void)
{
    return uvcd_class;
}

/**
 * Called by USBX to initialize class instance. Resources would be created here.
*/
UINT  uxd_uvc_initialize(const UX_SLAVE_CLASS_COMMAND *command)
{
    static char uvcd_main_semaphore_name[]  = "uvcd_main_semaphore";
    static char uvcd_main_thread_name[]     = "uvcd_main_thread";
    static char uvcd_event_thread_name[]    = "uvcd_event_thread";
    static char uvcd_event_semaphore_name[] = "uvcd_event_semaphore";
    static char uvcd_request_mutex_name[]   = "uvcd_request_mutex";
    UINT status;
    UX_SLAVE_CLASS *ux_class;

    /* Get the class container.  */
    ux_class = command->ux_slave_class_command_class_ptr;

    _ux_utility_memory_set(&uvcd_class_memory, 0, sizeof(uvcd_class_memory));
    _ux_utility_memory_set(uvcd_wrapper_pool_memory, 0, sizeof(uvcd_wrapper_pool_memory));

    /* Save the address of the STORAGE instance inside the STORAGE container.  */
    ux_class->ux_slave_class_instance = &uvcd_class_memory;

    uxd_uvc_class_set(ux_class);
    uxd_uvc_context_set(&uvcd_class_memory);

    ux_class->ux_slave_class_thread_stack = uvcd_main_thread_stack_memory;
    uvcd_class_memory.event_thread_stack  = uvcd_event_thread_stack_memory;
    uvcd_class_memory.request_pool        = uvcd_wrapper_pool_memory;
    uvcd_class_memory.request_num         = UVC_MAX_REQUEST_NUM;

    status = _ux_utility_semaphore_create(&uvcd_class_memory.class_thread_semaphore,
                                          uvcd_main_semaphore_name,
                                          0);

    if (status != 0U) {
        _ux_utility_print_uint5("_ux_device_class_uvc_initialize(): can't create semaphore, code 0x%X", status, 0, 0, 0, 0);
        status = UX_SEMAPHORE_ERROR;
    }

    if (status == 0U) {
        status = _ux_utility_thread_create(&ux_class->ux_slave_class_thread,
                                           uvcd_main_thread_name,
                                           uxd_uvc_thread,
                                           0,
                                           ux_class->ux_slave_class_thread_stack,
                                           MAIN_THREAD_STACK_SIZE,
                                           MAIN_THREAD_PRIORITY,
                                           MAIN_THREAD_PRIORITY,
                                           UX_NO_TIME_SLICE,
                                           TX_AUTO_ACTIVATE);

        if (status != 0U) {
            _ux_utility_print("_ux_device_class_uvc_initialize(): can't create thread.");
            status = UX_THREAD_ERROR;
        }
    }

    if (status == 0U) {
        status = _ux_utility_semaphore_create(&uvcd_class_memory.event_thread_semaphore, uvcd_event_semaphore_name, 0);
        if (status != 0U) {
            _ux_utility_print("_ux_device_class_uvc_initialize(): can't create event semaphore.");
            status = (UX_THREAD_ERROR);
        }
    }

    if (status == 0U) {
        /* request Mutex. */
        status = _ux_utility_mutex_create(&uvcd_class_memory.request_mutex, uvcd_request_mutex_name);
        if (status != 0U) {
            _ux_utility_print("_ux_device_class_uvc_initialize(): can't create mutex.");
            status = UX_MUTEX_ERROR;
        }
    }

    if (status == 0U) {
        status = _ux_utility_thread_create(&uvcd_class_memory.event_thread,
                                           uvcd_event_thread_name,
                                           uxd_uvc_event_thread,
                                           0,
                                           uvcd_class_memory.event_thread_stack,
                                           EVENT_THREAD_STACK_SIZE,
                                           EVENT_THREAD_PRIORITY,
                                           EVENT_THREAD_PRIORITY,
                                           UX_NO_TIME_SLICE,
                                           TX_AUTO_ACTIVATE);
        if (status != 0U) {
            _ux_utility_print("_ux_device_class_uvc_initialize(): can't create event thread.");
            status = UX_THREAD_ERROR;
        }
    }

    return (status);
}
/** @} */
