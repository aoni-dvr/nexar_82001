/**
 *  @file ux_device_class_uvc_send.c
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
 *  @details send functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

/** \addtogroup UVC_DEVICE_APIS
 *  @{
 * */
/**
 * This function is used if the UVC device supports multiple video stream interfaces and bulk transfer.
 * Application can use this function to send video data to the host with assigned stream ID.
 * @param StreamID [Input] Stream ID to send
 * @param pBuffer  [Input] The buffer to send
 * @param Size     [Input] The data size for send
 * @param Timeout  [Input] Timeout for this API
 * @param NoCopy   [Input] Indicate whether the data in buffer should be used directly
    - 0:  The data in buffer is copied before sending.
    - 1:  The data in buffer is used directly.
*/
UINT uxd_uvc_multi_payload_send(UINT32 StreamID, UINT8 *pBuffer, UINT32 Size, UINT32 Timeout, UINT32 NoCopy)
{
    UX_SLAVE_ENDPOINT *endpoint_in;
    const UX_SLAVE_DEVICE *device;
    const UX_SLAVE_INTERFACE *intf;
    UX_SLAVE_TRANSFER *transfer_request = NULL;
    ULONG local_requested_length;
    UINT status                   = 0;
    UINT found                    = 0;
    const UX_SLAVE_CLASS_UVC *ctx       = uxd_uvc_context_get();
    const UVCD_VS_INSTANCE *vs_instance = NULL;
    UINT8 *buffer_start_ptr = pBuffer;
    UINT32 buffer_index = 0;

    /* Get the pointer to the device.  */
    device = &_ux_system_slave->ux_system_slave_device;

    if (ctx != NULL) {
        vs_instance = ctx->first_vs_instance;
    }

    while (vs_instance != NULL) {
        if (vs_instance->stream_id == StreamID) {
            break;
        }
        vs_instance = vs_instance->next;
    }

    if (vs_instance == NULL) {
        status = UX_PARAMETER_ERROR;
    } else {
        intf = vs_instance->ux_interface;
        if (intf != NULL) {
            if (intf->ux_slave_interface_status == (ULONG)UX_USED) {
                endpoint_in = intf->ux_slave_interface_first_endpoint;
                while (endpoint_in != NULL) {
                    if (endpoint_in->ux_slave_endpoint_status == (ULONG)UX_USED) {
                        if (endpoint_in->ux_slave_endpoint_descriptor.bmAttributes == 0x03U) {
                            // interrupt endpoint, skip it
                            endpoint_in = endpoint_in->ux_slave_endpoint_next_endpoint;
                        } else {
                            found = 1;
                            break;
                        }
                    } else {
                        endpoint_in = endpoint_in->ux_slave_endpoint_next_endpoint;
                    }
                }

                if ((found == 1U) && (endpoint_in != NULL)) {
                    //dbg("Address = 0x%X", endpoint_in->ux_slave_endpoint_descriptor.bEndpointAddress);
                    transfer_request                                    = &endpoint_in->ux_slave_endpoint_transfer_request;
                    transfer_request->ux_slave_transfer_request_timeout = Timeout;
                } else {
                    _ux_utility_print("uxd_uvc_multi_payload_send(): No endpoint active.");
                    status = UX_NO_ED_AVAILABLE;
                }
            } else {
                _ux_utility_print("uxd_uvc_multi_payload_send(): No interface active.");
                status = UX_NO_ED_AVAILABLE;
            }
        }

        if ((found == 1U) && (status == 0U) && (transfer_request != NULL)) {
            if ((device->ux_slave_device_state == (ULONG)UX_DEVICE_CONFIGURED) && (Size == 0U)) {
                status = _ux_device_stack_transfer_request(transfer_request, 0, 0);
            } else {
                UCHAR *ptr = transfer_request->ux_slave_transfer_request_data_pointer;

                while ((device->ux_slave_device_state == (ULONG)UX_DEVICE_CONFIGURED) && (Size != 0U)) {
                    if (Size > transfer_request->max_buffer_size) {
                        local_requested_length = transfer_request->max_buffer_size;
                    } else {
                        local_requested_length = Size;
                    }

                    if (NoCopy == 0U) {
                        _ux_utility_memory_copy(transfer_request->ux_slave_transfer_request_data_pointer,
                                                &buffer_start_ptr[buffer_index], local_requested_length);
                    } else {
                        ptr                                                      = transfer_request->ux_slave_transfer_request_data_pointer;
                        transfer_request->ux_slave_transfer_request_data_pointer = &buffer_start_ptr[buffer_index];
                    }

                    status = _ux_device_stack_transfer_request(transfer_request, local_requested_length, local_requested_length);

                    if (NoCopy == 1U) {
                        transfer_request->ux_slave_transfer_request_data_pointer = ptr;
                    }

                    if (status == 0U) {
                        UINT32 byte_sent = transfer_request->ux_slave_transfer_request_actual_length;
                        buffer_index += byte_sent;
                        Size   -= byte_sent;
                    } else {
                        break;
                    }
                }
            }
        }
    }

    return status;
}

/**
 * Get maximum transfer buffer size for the video stream interface
*/
UINT32 uxd_uvc_max_xfer_length_get(UINT32 vs_id)
{
    const UX_SLAVE_ENDPOINT *endpoint_in;
    const UX_SLAVE_INTERFACE *intf;
    const UX_SLAVE_CLASS_UVC *ctx   = uxd_uvc_context_get();
    UINT found                = 0;
    UINT32 max_request_length = 0;
    const UVCD_VS_INSTANCE *uvc_vs_cur;

    uvc_vs_cur = ctx->first_vs_instance;
    while (uvc_vs_cur != NULL) {
        if (uvc_vs_cur->stream_id == vs_id) {
            found = 1;
            break;
        }
        uvc_vs_cur = uvc_vs_cur->next;
    }

    if ((found == 0U) || (uvc_vs_cur == NULL)) {
        _ux_utility_print_uint5("uxd_uvc_max_xfer_length_get(): no match stream ID (%d)", vs_id, 0, 0, 0, 0);
    } else {
        intf = uvc_vs_cur->ux_interface;
        if (intf != NULL) {
            if (intf->ux_slave_interface_status == (ULONG)UX_USED) {
                endpoint_in = intf->ux_slave_interface_first_endpoint;
                while (endpoint_in != NULL) {
                    if (endpoint_in->ux_slave_endpoint_status == (ULONG)UX_USED) {
                        if (endpoint_in->ux_slave_endpoint_descriptor.bmAttributes == 0x03U) {
                            endpoint_in = endpoint_in->ux_slave_endpoint_next_endpoint;
                            found       = 0;
                        } else {
                            found = 1;
                            break;
                        }
                    } else {
                        endpoint_in = endpoint_in->ux_slave_endpoint_next_endpoint;
                    }
                }

                if ((found == 1U) && (endpoint_in != NULL)) {
                    max_request_length = endpoint_in->ux_slave_endpoint_transfer_request.max_buffer_size;
                    //_ux_utility_print_uint5("uxd_uvc_max_xfer_length_get(): max request length = %d", max_request_length, 0, 0, 0, 0);
                } else {
                    _ux_utility_print("uxd_uvc_max_xfer_length_get(): No endpoint active.");
                }
            } else {
                _ux_utility_print("uxd_uvc_max_xfer_length_get(): No interface active.");
            }
        }
    }

    return max_request_length;
}

/**
 * Isochronous transfers ONLY.
 * Allocate transfer requests.
*/
UX_SLAVE_TRANSFER* uxd_uvc_xfer_request_allocate(UINT32 vs_id)
{
    UX_SLAVE_ENDPOINT *endpoint_in;
    const UX_SLAVE_INTERFACE *intf;
    UX_SLAVE_TRANSFER *transfer_request = NULL;
    UINT status                         = 0;
    UINT found                          = 0;
    UX_SLAVE_CLASS_UVC *ctx             = uxd_uvc_context_get();
    ULONG i                             = 0;
    const UVCD_VS_INSTANCE *uvc_vs_cur;

    if (ctx != NULL) {
        uvc_vs_cur = ctx->first_vs_instance;
        while (uvc_vs_cur != NULL) {
            if (uvc_vs_cur->stream_id == vs_id) {
                found = 1;
                break;
            }
            uvc_vs_cur = uvc_vs_cur->next;
        }

        if ((found == 0U) || (uvc_vs_cur == NULL)) {
            _ux_utility_print_uint5("uxd_uvc_xfer_request_allocate(): no match stream ID (%d)", vs_id, 0, 0, 0, 0);
        } else {
            intf = uvc_vs_cur->ux_interface;
            if (intf != NULL) {
                if (intf->ux_slave_interface_status == (ULONG)UX_USED) {
                    ULONG tmp_ep_state;
                    endpoint_in = intf->ux_slave_interface_first_endpoint;

                    // Pass misra-c
                    // Not decleare it as constant pointer, or below assignment will extend its capability.
                    tmp_ep_state = endpoint_in->ux_slave_endpoint_state;
                    endpoint_in->ux_slave_endpoint_state = tmp_ep_state;

                    while (endpoint_in != NULL) {
                        if (endpoint_in->ux_slave_endpoint_status == (ULONG)UX_USED) {
                            if (endpoint_in->ux_slave_endpoint_descriptor.bmAttributes == 0x03U) {
                                // interrupt endpoint, skip it
                                endpoint_in = endpoint_in->ux_slave_endpoint_next_endpoint;
                                found       = 0;
                            } else {
                                found = 1;
                                break;
                            }
                        } else {
                            endpoint_in = endpoint_in->ux_slave_endpoint_next_endpoint;
                        }
                    }

                    if ((found == 1U) && (endpoint_in != NULL)) {
                        //_ux_utility_print_uint5("Address = 0x%X", endpoint_in->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);

                        /* allocate new transfer_request */
                        status = _ux_utility_mutex_on_ex(&ctx->request_mutex, 500);
                        if (status != 0U) {
                            _ux_utility_print_uint5("uxd_uvc_xfer_request_allocate(): get mutex fail 0x%X.", status, 0, 0, 0, 0);
                        } else {
                            for (i = 0; i < ctx->request_num; i++) {
                                UX_SLAVE_REQUEST_WRAPPER * pReqeustPool = ctx->request_pool;
                                UX_SLAVE_REQUEST_WRAPPER *request_wrapper = &pReqeustPool[i];
                                if (request_wrapper->used == 0U) {
                                    transfer_request = &request_wrapper->transfer_request;
                                    _ux_utility_memory_set(transfer_request, 0, sizeof(UX_SLAVE_TRANSFER));
                                    request_wrapper->used                                = 1;
                                    transfer_request->ux_slave_transfer_request_endpoint = endpoint_in;
                                    break;
                                }
                            }
                            _ux_utility_mutex_off(&ctx->request_mutex);
                        }
                    } else {
                        _ux_utility_print("uxd_uvc_xfer_request_allocate(): No endpoint active.");
                    }
                } else {
                    _ux_utility_print("uxd_uvc_xfer_request_allocate(): No interface active.");
                }
            }
        }
    }

    return transfer_request;
}

/**
 * Isochronous transfers ONLY.
 * Free transfer requests.
*/
UINT uxd_uvc_xfer_request_free(const UX_SLAVE_TRANSFER *transfer_request)
{
    ULONG i;
    UINT32 rval             = 0;
    UX_SLAVE_CLASS_UVC *ctx = uxd_uvc_context_get();
    UINT uRet               = UX_SUCCESS;

    if (ctx == NULL) {
        uRet = UX_ERROR;
    } else {
        rval = _ux_utility_mutex_on_ex(&ctx->request_mutex, 500);
        if (rval != 0U) {
            _ux_utility_print_uint5("uxd_uvc_xfer_request_free(): get mutex fail 0x%X.", rval, 0, 0, 0, 0);
        } else {
            for (i = 0; i < ctx->request_num; i++) {
                UX_SLAVE_REQUEST_WRAPPER *request_wrapper = &ctx->request_pool[i];
                if (&request_wrapper->transfer_request == transfer_request) {
                    request_wrapper->used = 0;
                    break;
                }
            }
            _ux_utility_mutex_off(&ctx->request_mutex);
        }
    }

    return uRet;
}

/**
 * Isochronous transfers ONLY.
 * Fill transfer requests.
*/
UINT uxd_uvc_iso_transfer_fill(UX_SLAVE_TRANSFER *                     transfer_request,
                               VOID *                                  iso_req_head,
                               ULONG                                   iso_req_num,
                               ULONG                                   timeout,
                               ux_slave_transfer_request_completion_fn compl_fn)
{
    const UX_SLAVE_ENDPOINT *endpoint = transfer_request->ux_slave_transfer_request_endpoint;
    UINT32 status = UX_SUCCESS;
    UINT max_burst = 0, mult = 0;
    ULONG max_packet_size;


    if (_ux_system_slave->ux_system_slave_speed == (ULONG)UX_SUPER_SPEED_DEVICE) {
        max_burst = endpoint->ux_slave_ss_comp_descriptor.bMaxBurst + 1U;
        mult      = (1U + (endpoint->ux_slave_ss_comp_descriptor.bmAttributes & 0x03U));
    } else {
        max_burst = 1;
        mult      = (1U + ((endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize >> 11U) & 0x03U));
    }
    max_packet_size = endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize * max_burst * mult;
    // Note: Reserve it until it's useless.
    (VOID)max_packet_size;

    transfer_request->ux_slave_transfer_request_data_pointer        = NULL;
    transfer_request->ux_slave_transfer_request_requested_length    = 0;
    transfer_request->ux_slave_transfer_request_iso_request_head    = iso_req_head;
    transfer_request->ux_slave_transfer_request_iso_num             = iso_req_num;
    transfer_request->completion_function                           = compl_fn;
    transfer_request->ux_slave_transfer_request_timeout             = timeout;

    return status;
}

/**
 * Isochronous transfers ONLY.
 * Submit transfer requests.
*/
UINT uxd_uvc_iso_transfer_submit(UX_SLAVE_TRANSFER *transfer_request)
{
    UINT32 status;
    UINT32 requested_length = transfer_request->ux_slave_transfer_request_requested_length;

    status = _ux_device_stack_transfer_request(transfer_request, requested_length, requested_length);

    return status;
}
/** @} */
