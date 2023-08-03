/**
 *  @file ux_device_class_uvc_commit_control.c
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
 *  @details commit control functions of the uvc device class
 */

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_uvc.h"
#include "ux_device_stack.h"

static void dbg(const char *str)
{
	_ux_utility_print_uint5(str, 0, 0, 0, 0, 0);
}

UINT _ux_device_class_uvc_commit_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq)
{
    UINT8 *data  = transfer_request->ux_slave_transfer_request_data_pointer;
    UINT16 if_id = (sreq->wIndex & 0x00FFU);
    UINT uRet;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
    {
        const uvcd_probe_control_t *pc;
        UINT16 vs_id = if_id - 1U; // Assume 1st interface is VC
        dbg("Commit Control: SET_CUR");
        uvcd_config_probe_control(data, sreq->wLength);
        pc = uvcd_get_probe_control();
        uvcd_config_mjpeg_resolution(pc->bFormatIndex, pc->bFrameIndex);
        uvcd_notify_encode_start((UINT32)vs_id);     // Assume 1st interface is VC
        uRet = UX_SUCCESS;
        break;
    }

    case UVC_GET_CUR:
    {
        const uvcd_probe_control_t *pc = uvcd_get_probe_control();
        dbg("Commit Control: GET_CUR");
        uvcd_copy_probe_control(pc, data);
        uRet = _ux_device_stack_transfer_request(transfer_request, sreq->wLength, sreq->wLength);
        break;
    }

    case UVC_GET_MIN:
    case UVC_GET_MAX:
    case UVC_GET_RES:
    case UVC_GET_DEF:
    case UVC_GET_LEN:
    case UVC_GET_INFO:
    default:
        _ux_utility_print_uint5("process_commit_control() : Unknown VS request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        uvcd_stall_controll_endpoint();
        uRet = UX_TRANSFER_STALLED;
        break;
    }
    return uRet;
}

UINT _ux_device_class_uvc_still_commit_control(UX_SLAVE_TRANSFER *transfer_request, const USB_SETUP_REQUEST_S *sreq)
{
    UINT uRet;

    (VOID)transfer_request;

    switch (sreq->bRequest) {
    case UVC_SET_CUR:
    {
        //uvcd_probe_control_t *pc = NULL;
        dbg("Still Commit Control: SET_CUR");
        uRet = UX_SUCCESS;
        break;
    }

    case UVC_GET_CUR:
    case UVC_GET_MIN:
    case UVC_GET_MAX:
    case UVC_GET_RES:
    case UVC_GET_DEF:
    case UVC_GET_LEN:
    case UVC_GET_INFO:
    default:
        _ux_utility_print_uint5("process_still_commit_control() : Unknown VS request (0x%X)", sreq->bRequest, 0, 0, 0, 0);
        uvcd_stall_controll_endpoint();
        uRet = UX_TRANSFER_STALLED;
        break;
    }
    return uRet;
}

