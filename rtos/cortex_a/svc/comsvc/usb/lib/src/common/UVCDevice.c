/**
 *  @file UVCDevice.c
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
 *  @details USB Device Video class application.
 */

#include <AmbaTypes.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSB_ErrCode.h>
#include <usbx/ux_api.h>
#include <usbx/ux_device_stack.h>
#include "AmbaUSBD_Uvc.h"
#include "../../../../../apps/icam/cardv/platform.h"

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define AppUsb_Memset AmbaWrap_memset
#define AppUsb_Memcpy AmbaWrap_memcpy
#endif

#define UVC_10_HEADER_EOH   0x80
#define UVC_10_HEADER_EOF   0x02
#define UVC_10_HEADER_FID   0x01
#define UVC_XU_MAX_DATA_LENGTH      12

#define UVC_MAX_PAYLOAD_SIZE (800U * 1024U)
#define UVCD_MAX_VS_NUMBER   1U

static UINT8 uvcd_send_buffer[UVCD_MAX_VS_NUMBER][UVC_MAX_PAYLOAD_SIZE] __attribute__ ((aligned(32))) __attribute__((section(".bss.noinit")));
static UINT8 uvcd_data_buffer[UVCD_MAX_VS_NUMBER][UVC_MAX_PAYLOAD_SIZE] __attribute__ ((aligned(32))) __attribute__((section(".bss.noinit")));
static USBD_UVC_HOST_CONFIG_s uvcd_host_config[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT8  uvcd_header_bfh[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT32 uvcd_send_ready[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT32 uvcd_send_buffer_size = UVC_MAX_PAYLOAD_SIZE;
static UINT8 uvc_send_ready = 0;

void AppUvcd_SendFrame(UINT32 id, UINT8 *start, UINT32 size)
{
    // H264 & UVC Bulk only.
    // Each Bulk transfer just requires one UVC header.
    UINT32  Index = 0;
    UINT8   header_size = 2;
    UINT32  timeout = 100;
    UINT32  no_copy = 1;
    UINT32  max_data_size = AmbaUSBD_UVCGetProbeMaxPayloadXferSize() - header_size;
    UINT32  pos = 0;
    UINT32  bytes_to_copy = 0;
    UINT8   *ptr_src = uvcd_data_buffer[id];
    UINT8   *ptr_dst = uvcd_send_buffer[id];

    /* toggle FID first */
    if ((uvcd_header_bfh[id] & UVC_10_HEADER_FID) != 0U) {
        uvcd_header_bfh[id] &= ~(UVC_10_HEADER_FID);
    } else {
        uvcd_header_bfh[id] |= UVC_10_HEADER_FID;
    }

    if (size > uvcd_send_buffer_size) {
        return;
    }

    (VOID)AppUsb_Memcpy(&ptr_src[Index], start, size);
    // Each bulk transfer requires one UVC header if the size exceeds maximum payload size.
    while (pos < size) {
        if ((pos + max_data_size) >= size) {
            /* mark EOF field */
            uvcd_header_bfh[id] |= UVC_10_HEADER_EOF;
            bytes_to_copy = size - pos;
        } else {
            /* clear EOF field */
            if ((uvcd_header_bfh[id] & UVC_10_HEADER_EOF) != 0U) {
                uvcd_header_bfh[id] &= ~(UVC_10_HEADER_EOF);
            }
            bytes_to_copy = max_data_size;
        }
        ptr_dst = uvcd_send_buffer[id];
        ptr_dst[0] = header_size;
        ptr_dst[1] = uvcd_header_bfh[id];

        (VOID)AppUsb_Memcpy(&ptr_dst[header_size], ptr_src, bytes_to_copy);

        ptr_src = &ptr_src[bytes_to_copy];
        pos += bytes_to_copy;
        if (AmbaUSBD_UVCBulkSend(id, ptr_dst, bytes_to_copy + header_size, timeout, no_copy) != USB_ERR_SUCCESS) {
            debug_line("uvcd_send_bulk_frame(): can't send video frame.");
            break;
        }
    }
}

extern int uvc_start(int vin);
static int vin_index = 0;
static UINT32 AppUvcd_EncodeStart(UINT32 Index)
{
    AmbaUSBD_UVCGetHostConfig(&uvcd_host_config[Index]);
    debug_line("Host Select format %dx%d@%dfps", uvcd_host_config[Index].Width, uvcd_host_config[Index].Height, uvcd_host_config[Index].Framerate);
    uvc_send_ready = 1;
    uvc_start(vin_index);

    return 0;
}



extern int uvc_stop(void);
static UINT32 AppUvcd_EncodeStop(UINT32 Index)
{
    debug_line("[UVCD] Host wants to Stop encode, VS index = %d", Index);    
    uvc_stop();
    uvc_send_ready = 0;

    return 0;
}

static UINT32 AppUvcd_PuSetup(UINT32 Index)
{
    debug_line("PU setup, Index = %d", Index);
    return 0;
}

static UINT32 AppUvcd_ItSetup(UINT32 Index)
{
    debug_line("PU setup, Index = %d", Index);
    return 0;
}

static UINT32 AppUvcd_XuHandler(UINT32 Selector, UINT8 *Buffer, UINT32 Size, UINT32 Length)
{
    static UINT8 uvcd_xu_data_buffer[UVC_XU_MAX_DATA_LENGTH] __attribute__((section(".bss.noinit")));
	UINT32 request = Selector & 0x0FFU;

	switch(request) {
		case UVC_SET_CUR:
    		(VOID)AppUsb_Memcpy(uvcd_xu_data_buffer, Buffer, Length);
			break;
		case UVC_GET_CUR:
    		(VOID)AppUsb_Memcpy(Buffer, uvcd_xu_data_buffer, Length);
			break;
		case UVC_GET_INFO:
			Buffer[0] = (UINT8)(0x01U | 0x02U); // GET/SET supported only
			Buffer[1] = 0;
			Buffer[2] = 0;
			Buffer[3] = 0;
			break;
		case UVC_GET_MIN:
			(VOID)AppUsb_Memset(Buffer, 0, Size);
			break;
		case UVC_GET_MAX:
			(VOID)AppUsb_Memset(Buffer, 0xFF, Size);
			break;
		case UVC_GET_RES:
			break;
		case UVC_GET_LEN:
		    {
		        //keep misra-c happy
		        UINT32 length = UVC_XU_MAX_DATA_LENGTH;
		        (VOID)AppUsb_Memset(Buffer, 0, Size);
		        Buffer[0] = (UINT8)length;
		        Buffer[1] = (UINT8)(length >> 8U);
		    }
			break;
		case UVC_GET_DEF:
			(VOID)AppUsb_Memset(Buffer, 0, Size);
			break;
		default:
			debug_line("UVC XU: Unknown Selector = 0x%X", request);
			break;
	}
    return 0;
}

static UINT32 AppUvcd_ResourceCreate(void)
{
    USBD_UVC_EVENT_CALLBACK_s cbs = {NULL};    

    cbs.EncodeStart = AppUvcd_EncodeStart;
    cbs.EncodeStop = AppUvcd_EncodeStop;
    cbs.PuSet = AppUvcd_PuSetup;
    cbs.ItSet = AppUvcd_ItSetup;
    cbs.XuHandler = AppUvcd_XuHandler;

    AmbaUSBD_UVCRegisterCallback(&cbs);
    //AmbaUSBD_UVC_SetInputTerminalAttribute(UVC_SUPPORT_IT_CONTROLS);
    //AmbaUSBD_UVC_SetProcessingUnitAttribute(UVC_SUPPORT_PU_CONTROLS);
    AmbaUSBD_UVC_SetProbeMaxPayloadXferSize(UVC_MAX_PAYLOAD_SIZE);
    debug_line("AppUvcd_ResourceCreate(): uvc ut initialized.");

    return 0;
}

static void AppUvcd_InitZero(void)
{
    UINT32 i = 0;
    for (i = 0; i < UVCD_MAX_VS_NUMBER; i++) {
        uvcd_send_ready[i] = 0;
        (VOID)AppUsb_Memset(&uvcd_host_config[i], 0, sizeof(USBD_UVC_HOST_CONFIG_s));
        uvcd_header_bfh[i] = UVC_10_HEADER_EOH;
    }

    return;
}

int AppUvcd_Init(int vin)
{
    static UINT8 init = 0;

    vin_index = vin;
    AppUvcd_InitZero();
    if (init == 0U) {
        AppUvcd_InitZero();
        (VOID)AppUvcd_ResourceCreate();
        init = 1;
    }

    return 0;
}

int AppUvcd_SendCheck(void)
{
    return uvc_send_ready;
}

void AppUvcd_Release(void)
{
    // To pass misra-c "function lacks side_effects" issue.
    static UINT32 deinit = 0;
    if (deinit == 0U) {
        // no resource destroy needed
        deinit = 1;
    }
    return;
}

