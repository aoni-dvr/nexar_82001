/**
 *  @file AppUSB_UVCDevice.c
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
 *  @details USB Device Video class application.
 */
/**
 * \page page5 UVC Device application
*/
#ifdef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif
#ifndef APP_USB_H
#include "AppUSB.h"
#endif
#include <AmbaUSB_ErrCode.h>
#ifndef UX_API_H
#include <usbx/ux_api.h>
#endif

#ifndef AMBAUSBD_UVC_H
#include "device_classes/uvc/AmbaUSBD_Uvc.h"
#endif
#include "AmbaMisraFix.h"

#ifndef AMBA_USB_UVCD_H
#include "AppUSB_UVCDevice.h"
#endif

// Note: For Windows XP/7, the data size can't exceed 12 bytes for XU.
//       Otherwise, Windows will reject to recognize this UVC device.
//       Maximum is 1024 bytes.
#define UVC_XU_MAX_DATA_LENGTH            12U

#define UVC_10_HEADER_EOH                 0x80U
#define UVC_10_HEADER_STI                 0x20U
#define UVC_10_HEADER_EOF                 0x02U
#define UVC_10_HEADER_FID                 0x01U

#define UVC_MAX_PAYLOAD_SIZE              (1024U * 1024U)
#define UVC_MAX_DATA_SIZE                 (1024U * 1024U * 4U)
#define UVCD_MAX_VS_NUMBER                UVC_STREAM_NUMBER
#define UVCD_SEND_THREAD_STACK_SIZE       (1024U*4U)
#define UVCD_MJPEG_THREAD_STACK_SIZE      (1024U*4U)

#define UVCD_MESSGE_QUEUE_NUMBER          (128UL)

/**
 * Message for video frame data information
*/
typedef struct {
    UINT8 *DataBuffer;     //!< video data buffer pointer
    UINT32 DataBufferSize; //!< video data buffer size
    UINT32 FrameStartPos;  //!< video frame start position in data buffer
    UINT32 FrameSize;      //!< video frame size
    UINT32 FrameType;      //!< video frame type
    UINT32 StreamID;       //!< Steam ID which the video frame belongs
} UVCD_MESSAGE_BODY_s;

#define UVCD_MESSAGE_QUEUE_SIZE (sizeof(UVCD_MESSAGE_BODY_s) * UVCD_MESSGE_QUEUE_NUMBER)

static AMBA_KAL_TASK_t thread_send_data __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t thread_push_mjpeg[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));

static UINT8 message_queue_buffer[UVCD_MESSAGE_QUEUE_SIZE] __attribute__((section(".bss.noinit")));
static UINT8 mjpeg_buffer[UVCD_MAX_VS_NUMBER][UVC_MAX_DATA_SIZE] __attribute__((section(".bss.noinit")));
static UINT8 still_mjpeg_buffer[UVC_MAX_DATA_SIZE] __attribute__((section(".bss.noinit")));
static UINT32 flag_send_ready[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT32 flag_still_capture_ready[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT8  uvc_header_bfh[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT32 mjpeg_size[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT32 still_mjpeg_size __attribute__((section(".bss.noinit")));
static UINT32 encode_start_time __attribute__((section(".bss.noinit")));

static AMBA_KAL_MSG_QUEUE_t   message_queue __attribute__((section(".bss.noinit")));
static uvcd_host_config_t     uvc_host_config[UVCD_MAX_VS_NUMBER] __attribute__((section(".bss.noinit")));
static UINT32 flag_app_uvcd_stop __attribute__((section(".bss.noinit")));

static void init_zero(void)
{
    UINT32 i;
    for (i = 0; i < UVCD_MAX_VS_NUMBER; i++) {
        flag_send_ready[i] = 0;
        flag_still_capture_ready[i] = 0;
        AppUsb_MemoryZeroSet(&uvc_host_config[i], sizeof(uvcd_host_config_t));
        mjpeg_size[i] = 0;
        AppUsb_MemoryZeroSet(&thread_push_mjpeg[i], sizeof(AMBA_KAL_TASK_t));
        uvc_header_bfh[i] = UVC_10_HEADER_EOH;
    }
    AppUsb_MemoryZeroSet(&thread_send_data, sizeof(thread_send_data));
    AppUsb_MemoryZeroSet(message_queue_buffer, sizeof(message_queue_buffer));
    AppUsb_MemoryZeroSet(&message_queue, sizeof(message_queue));
    encode_start_time = 0;
    return;
}

static void data_ushort_put(UINT8 *pData, UINT32 Value)
{
    if (pData != NULL) {
        pData[0] = (UINT8)Value;
        pData[1] = (UINT8)(Value >> 8U);
    }
}

static void data_short_put(UINT8 *pData, INT16 Value)
{
    if (pData != NULL) {
        UINT16 u16_value = (UINT16)Value;
        pData[0] = (UINT8)u16_value;
        pData[1] = (UINT8)(u16_value >> 8U);
    }
}

static INT16 data_short_get(const UINT8 *pData)
{
    INT16 Value = 0;
    if (pData != NULL) {
        UINT32 x = pData[0];
        UINT32 y = pData[1];
        y = y << 8U;
        x += y;
        Value = (INT16)x;
    }
    return Value;
}

static UINT still_capture_start(UINT Index)
{
    AppUsb_PrintUInt5("[UVCD] Still Capture Start, index = %d", Index, 0, 0, 0, 0);
    flag_still_capture_ready[Index] = 1;
    return 0;
}

static UINT encode_start(UINT Index)
{
    UINT32  bytes_read;
    APP_USB_FILE_INFO stat;
    UINT32 uret = 0;
    UINT32 width, height;

    AppUsb_PrintUInt5("[UVCD] Host wants to Start encode, VS index = %d", Index, 0, 0, 0, 0);

    if (uxd_uvc_host_config_get(Index, &uvc_host_config[Index]) == USB_ERR_SUCCESS) {
        if (uvc_host_config[Index].format_index == 0x01U) {
            static char fnames[3][64] = {
                "c:\\uvc_1920x1080_1.jpg",
                "c:\\uvc_1280x720_1.jpg",
                "c:\\uvc_848x480_1.jpg"
            };
            const char *fname;
            AMBA_FS_FILE *f = NULL;

            if (Index == 0U) {
                // stream 0, MJPEG format
                switch (uvc_host_config[Index].frame_index) {
                case 1:
                    width = 848;
                    height = 480;
                    break;
                case 2:
                    width = 1280;
                    height = 720;
                    break;
                case 3:
                    width = 1920;
                    height = 1080;
                    break;
                default:
                    width = 848;
                    height = 480;
                    break;
                }
            } else {
                // stream 1, MJPEG format
                width = 1920;
                height = 1080;
            }

            // mjpeg route
            AppUsb_PrintUInt5("Host Select MJPEG format %dx%d", width, height, 0, 0, 0);

            if ((width == 1920U) && (height == 1080U)) {
                fname = fnames[0];
            } else if ((width == 1280U) && (height == 720U)) {
                fname = fnames[1];
            } else if ((width == 848U) && (height == 480U)) {
                fname = fnames[2];
            } else {
                fname = NULL;
            }

            if (fname != NULL) {
                UINT32 func_uret = AppUsb_FSFileInfoGet(fname, &stat);
                if (func_uret != 0U) {
                    AppUsb_PrintStr5("encode_start(): %s doesn't exist.", fname, NULL, NULL, NULL, NULL);
                    uret = USB_ERR_FAIL;
                } else {
                    (void)AmbaFS_FileOpen(fname, "r", &f);
                    if(f == NULL) {
                        AppUsb_PrintStr5("encode_start(): %s doesn't exist.", fname, NULL, NULL, NULL, NULL);
                        uret = USB_ERR_FAIL;
                    } else {
                        UINT32 size_c;
                        AmbaMisra_TypeCast32(&size_c, &stat.Size);
                        (void)AmbaFS_FileRead(mjpeg_buffer[Index], 1U, size_c, f, &bytes_read);
                        if (bytes_read != stat.Size) {
                            if (AmbaFS_FileClose(f) != 0U) {
                                // ignore error
                            }
                            AppUsb_PrintUInt5("encode_start(): File read error, read %u != file size %u ", bytes_read, (UINT)stat.Size, 0, 0, 0);
                            uret = USB_ERR_FAIL;
                        } else {
                            mjpeg_size[Index] = (UINT32)stat.Size;
                            if (AmbaFS_FileClose(f) != 0U) {
                                // ignore error
                            }
                            flag_send_ready[Index] = 1;
                        }
                    }
                }
            }

        } else {
            // for h264, read data from file doesn't make sense
            // application should call AppUvcd_PushVideoFrame() to send h264 frames.
            AppUsb_PrintUInt5("--------------------------------------------------------------", 0, 0, 0, 0, 0);
            AppUsb_PrintUInt5("[UVCD] H264 Format. ", 0, 0, 0, 0, 0);
            AppUsb_PrintUInt5("[UVCD] Application should call AmbaUSBD_UVCBulkSend() to send h264 frames", 0, 0, 0, 0, 0);
            AppUsb_PrintUInt5("       when available.", 0, 0, 0, 0, 0);
            AppUsb_PrintUInt5("--------------------------------------------------------------", 0, 0, 0, 0, 0);
            flag_send_ready[Index] = 1;
        }
    } else {
        uret = USB_ERR_FAIL;
    }

    if (AmbaKAL_GetSysTickCount(&encode_start_time) != 0U) {
        // should not happen
    }
    return uret;
}

static UINT encode_stop(UINT Index)
{
    UINT32 encode_stop_time = 0;
    if (AmbaKAL_GetSysTickCount(&encode_start_time) != 0U) {
        // should not happen
    }
    if ((encode_stop_time - encode_start_time)  < 500U) {
        // For MAC OS, the start condition is Probe Commit + Endpoint Clear Stall
        // For Windows OS, the start condition is Proble Commit
        // To keep maximum OS compatibility, if encode_stop() is called right after encode_start(),
        // we can assume the UVC Host is MAC OS and don't stop encode.
        // Please refer to UVC application note for detailed information.
        AppUsb_PrintUInt5("[UVCD] Host wants to Stop encode, VS index = %d (false alarm, MAC OS?)", Index, 0, 0, 0, 0);
    } else {
        AppUsb_PrintUInt5("[UVCD] Host wants to Stop encode, VS index = %d", Index, 0, 0, 0, 0);
        if (Index < UVCD_MAX_VS_NUMBER) {
            flag_send_ready[Index] = 0;
        }
    }
    return 0;

}

static UINT32 extension_unit_max_length_get(void)
{
    return UVC_XU_MAX_DATA_LENGTH;
}

static UINT32 extension_unit_request_process(const USB_SETUP_REQUEST_S *pSetupRequest, UINT8 *pBuffer, UINT32 BufferSize)
{
    static UINT8 xu_data_buffer[UVC_XU_MAX_DATA_LENGTH] __attribute__((section(".bss.noinit")));
    UINT32 uret = 0;

    if (extension_unit_max_length_get() > 12U) {
        AppUsb_PrintUInt5("=============================================", 0, 0, 0, 0, 0);
        AppUsb_PrintUInt5("[UVCD] XU data lenth %d over 12 bytes.", UVC_XU_MAX_DATA_LENGTH, 0, 0, 0, 0);
        AppUsb_PrintUInt5("       Windows XP can't accept it.", 0, 0, 0, 0, 0);
        AppUsb_PrintUInt5("=============================================", 0, 0, 0, 0, 0);
    }

    switch(pSetupRequest->bRequest) {
    case UVC_SET_CUR:
        //DbgPrint("UVC XU Control : SET_CUR.");
        AppUsb_MemoryCopy(xu_data_buffer, pBuffer, pSetupRequest->wLength);
        //AmbaPrint("    %d %d %d %d", buf[0], buf[1], buf[2], buf[3]);
        break;
    case UVC_GET_CUR:
        //DbgPrint("UVC XU Control : GET_CUR.");
        AppUsb_MemoryCopy(pBuffer, xu_data_buffer, pSetupRequest->wLength);
        break;
    case UVC_GET_INFO:
        //DbgPrint("UVC XU Control : GET_INFO.");
        pBuffer[0] = 0x03U; // GET/SET supported only
        pBuffer[1] = 0;
        pBuffer[2] = 0;
        pBuffer[3] = 0;
        break;
    case UVC_GET_MIN:
        //DbgPrint("UVC XU Control : GET_MIN.");
        AppUsb_MemoryZeroSet(pBuffer, BufferSize);
        break;
    case UVC_GET_MAX:
        //DbgPrint("UVC XU Control : GET_MAX.");
        AppUsb_MemorySet(pBuffer, 255, BufferSize);
        break;
    case UVC_GET_RES:
        //DbgPrint("UVC XU Control : GET_RES.");
        break;
    case UVC_GET_LEN:
        //DbgPrint("UVC XU Control : GET_LEN.");
    {
        //keep misra-c happy
        UINT32 length = UVC_XU_MAX_DATA_LENGTH;
        AppUsb_MemoryZeroSet(pBuffer, BufferSize);
        pBuffer[0] = (UINT8)length;
        pBuffer[1] = (UINT8)(length >> 8U);
    }
    break;
    case UVC_GET_DEF:
        //DbgPrint("UVC XU Control : GET_DEF.");
        AppUsb_MemoryZeroSet(pBuffer, BufferSize);
        break;
    default:
        AppUsb_PrintUInt5("UVC XU: Unknown Selector = 0x%X", pSetupRequest->bRequest, 0, 0, 0, 0);
        uret = UVC_VC_ER_CD_INVALID_REQUEST;
        break;
    }
    return uret;
}

typedef struct {
    INT16 MaximumValue;
    INT16 MinimumValue;
    INT16 DefaultValue;
    INT16 CurrentValue;
} PU_BRIGHTNESS_s;

static UINT32 brightness_control_process(const USB_SETUP_REQUEST_S *pSetupRequest, UINT8 *pBuffer, UINT32 BufferSize)
{
    INT16 brightness = 0;
    UINT8 *data      = pBuffer;
    UINT32 uret      = 0;

    static PU_BRIGHTNESS_s brightness_setting = {256, -256, 0, 0};

    if (BufferSize < 2U) {
        uret = UVC_VC_ER_CD_WRONG_STATE;
    } else {

        switch (pSetupRequest->bRequest) {
        case UVC_SET_CUR:
            brightness     = data_short_get(data);
            AppUsb_PrintInt5("Brightness Control : UVC_SET_CUR %d ", brightness, 0, 0, 0, 0);
            if ((brightness > brightness_setting.MaximumValue) || (brightness < brightness_setting.MinimumValue)) {
                uret = UVC_VC_ER_CD_OUT_OF_RANGE;
            } else {
                brightness_setting.CurrentValue = brightness;
            }
            break;
        case UVC_GET_CUR:
            //AppUsb_PrintUInt5("Brightness Control : UVC_GET_CUR.", 0, 0, 0, 0, 0);
            brightness = brightness_setting.CurrentValue;
            data_short_put(data, brightness);
            break;

        case UVC_GET_INFO:
            //AppUsb_PrintUInt5("Brightness Control : GET_INFO.", 0, 0, 0, 0, 0);
            data_ushort_put(data, (UVC_GET_SUPPORT | UVC_SET_SUPPORT));
            break;

        case UVC_GET_MIN:
            //AppUsb_PrintUInt5("Brightness Control : GET_MIN.", 0, 0, 0, 0, 0);
            data_short_put(data, brightness_setting.MinimumValue);
            break;

        case UVC_GET_MAX:
            //AppUsb_PrintUInt5("Brightness Control : GET_MAX.", 0, 0, 0, 0, 0);
            data_short_put(data, brightness_setting.MaximumValue);
            break;

        case UVC_GET_RES:
            //AppUsb_PrintUInt5("Brightness Control : GET_RES.", 0, 0, 0, 0, 0);
            data_short_put(data, 1);
            break;
        case UVC_GET_DEF:
            //AppUsb_PrintUInt5("Brightness Control : GET_DEF.", 0, 0, 0, 0, 0);
            data_short_put(data, brightness_setting.DefaultValue);
            break;
        default:
            AppUsb_PrintUInt5("brightness_control_process() : Unknown bRequest = 0x%X", pSetupRequest->bRequest, 0, 0, 0, 0);
            uret = UVC_VC_ER_CD_INVALID_REQUEST;
            break;
        }
    }

    return uret;
}

static UINT32 processing_unit_request_process(const USB_SETUP_REQUEST_S *pSetupRequest, UINT8 *pBuffer, UINT32 BufferSize)
{
    UINT32 cs_id = (((UINT32)pSetupRequest->wValue >> 8U)) & 0x0FFU;
    UINT uret;
    switch (cs_id) {
    case UVC_PU_BRIGHTNESS_CONTROL:
        //AppUsb_PrintUInt5("Brightness Control Selector.", 0, 0, 0, 0, 0);
        uret = brightness_control_process(pSetupRequest, pBuffer, BufferSize);
        break;
    default:
        AppUsb_PrintUInt5("Unknown PU Control Selector(0x%X)", cs_id, 0, 0, 0, 0);
        uret = UVC_VC_ER_CD_INVALID_CONTROL;
        break;
    }
    return uret;
}

static UINT32 request_handler(const USB_SETUP_REQUEST_S *pSetupRequest, UINT8 *pBuffer, UINT32 BufferSize)
{
    UINT32 uret = 0;

    (void)pSetupRequest;
    (void)pBuffer;
    (void)BufferSize;

    if ((pSetupRequest == NULL) || (pBuffer == NULL)) {
        uret = UVC_VC_ER_CD_INVALID_REQUEST;
    } else {
        UINT16 UnitID = (pSetupRequest->wIndex >> 8U);
        if (UnitID == UVC_IT_ID) {
            uret = UVC_VC_ER_CD_INVALID_REQUEST;
        } else if (UnitID == UVC_PU_ID) {
            uret = processing_unit_request_process(pSetupRequest, pBuffer, BufferSize);
        } else if (UnitID == UVC_XU_ID) {
            uret = extension_unit_request_process(pSetupRequest, pBuffer, BufferSize);
        } else {
            // streaming interface, let stack handle it.
            uret = 0xFFFFFFFFU;
        }
    }

    return uret;
}


//
//    |-------| --> buffer address
//    |       |
//    |       |
//    |       | --> start_pos
//    |       |
//    |       |
//    |-------| --> buffer size

static void bulk_frame_send(UINT32 StreamID, const UINT32 FrameStartPos, UINT32 FrameSize, const UINT8 *pBuffer, UINT32 BufferSize, UINT32 FlagStill)
{
    // H264 & UVC Bulk only.
    // Each Bulk transfer just requires one UVC header.
    static UINT8 data_buffer[UVCD_MAX_VS_NUMBER][UVC_MAX_DATA_SIZE] __attribute__ ((aligned(64))) __attribute__((section(".bss.noinit")));
    static UINT8 send_buffer[UVCD_MAX_VS_NUMBER][UVC_MAX_PAYLOAD_SIZE] __attribute__ ((aligned(64))) __attribute__((section(".bss.noinit")));
    //static UINT32 send_buffer_size = UVC_MAX_PAYLOAD_SIZE;

    UINT32  index = 0;
    UINT8   header_size = 2;
    UINT32  timeout = 100;
    UINT32  no_copy = 1;
    UINT32  max_data_size = UVC_MAX_PAYLOAD_SIZE;
    UINT32  pos = 0;
    UINT32  bytes_to_copy = 0;
    UINT8   *ptr_src = data_buffer[StreamID];
    UINT8   *ptr_dst = send_buffer[StreamID];

    if (uxd_uvc_probe_max_xfer_size_get(StreamID, &max_data_size) == 0U) {
        // ?
    }
    max_data_size = max_data_size - header_size;

    /* toggle FID first */
    if ((uvc_header_bfh[StreamID] & UVC_10_HEADER_FID) != 0U) {
        uvc_header_bfh[StreamID] &= ~(UVC_10_HEADER_FID);
    } else {
        uvc_header_bfh[StreamID] |= UVC_10_HEADER_FID;
    }

    if (FlagStill != 0U) {
        uvc_header_bfh[StreamID] |= UVC_10_HEADER_STI;
    } else {
        uvc_header_bfh[StreamID] &= ~(UVC_10_HEADER_STI);
    }

    if (FrameSize > UVC_MAX_DATA_SIZE) {
        AppUsb_PrintUInt5("bulk_frame_send(): video data size %d > buffer size %d", FrameSize, UVC_MAX_DATA_SIZE, 0, 0, 0);
    } else {
        // copy data
        if ((FrameStartPos + FrameSize) > BufferSize) {
            UINT32 part_size = BufferSize - FrameStartPos;
            AppUsb_PrintUInt5("bulk_frame_send(): data wrap-around", 0, 0, 0, 0, 0);
            // wrap-around
            AppUsb_MemoryCopy(&ptr_src[index], &pBuffer[FrameStartPos], part_size);
            AppUsb_MemoryCopy(&ptr_src[index + part_size], pBuffer, FrameSize - part_size);
        } else {
            AppUsb_MemoryCopy(&ptr_src[index], &pBuffer[FrameStartPos], FrameSize);
        }

        // Each bulk transfer requires one UVC header if the size exceeds maximum payload size.
        while (pos < FrameSize) {
            if ((pos + max_data_size) >= FrameSize) {
                /* mark EOF field */
                uvc_header_bfh[StreamID] |= UVC_10_HEADER_EOF;
                bytes_to_copy = FrameSize - pos;
            } else {
                /* clear EOF field */
                if ((uvc_header_bfh[StreamID] & UVC_10_HEADER_EOF) != 0U) {
                    uvc_header_bfh[StreamID] &= ~(UVC_10_HEADER_EOF);
                }
                bytes_to_copy = max_data_size;
            }
            ptr_dst = send_buffer[StreamID];
            ptr_dst[0] = header_size;
            ptr_dst[1] = uvc_header_bfh[StreamID];

            AppUsb_MemoryCopy(&ptr_dst[header_size], ptr_src, bytes_to_copy);

            ptr_src = &ptr_src[bytes_to_copy];
            pos += bytes_to_copy;
            if (uxd_uvc_multi_payload_send(StreamID, ptr_dst, bytes_to_copy + header_size, timeout, no_copy) != USB_ERR_SUCCESS) {
                AppUsb_PrintUInt5("bulk_frame_send(): can't send video frame.", 0, 0, 0, 0, 0);
                break;
            }
        }
    }
}

static void *thread_send_func(void *Argv)
{
    UVCD_MESSAGE_BODY_s msg = {NULL, 0, 0, 0, 0, 0};
    UINT32 *ptr_stream_id;
    UINT32 stream_id;

    AmbaMisra_TouchUnused(Argv); // pass vcast check
    AmbaMisra_TypeCast32(&ptr_stream_id, &Argv);
    AmbaMisra_TouchUnused(ptr_stream_id); // pass vcast check
    stream_id = *ptr_stream_id;

    while (flag_app_uvcd_stop == 0U) {

        if (OK == AmbaKAL_MsgQueueReceive(&message_queue, &msg, 10)) {

            if (flag_send_ready[msg.StreamID] != 1U) {
                continue;
            }

            bulk_frame_send(msg.StreamID, msg.FrameStartPos, msg.FrameSize, msg.DataBuffer, msg.DataBufferSize, 0);

            if (uvc_host_config[msg.StreamID].format_index == 0x01U) {
                // MJPEG format. Nothing to do.
            } else {
                // clear h264 fifo here
            }
        } else if (flag_still_capture_ready[stream_id] != 0U) {

            const uvcd_still_probe_control_t *still_probe_config = uxd_uvc_still_probe_control_get();
            const UINT8 *ptr = still_mjpeg_buffer;
            UINT32 size = still_mjpeg_size;
            AppUsb_PrintUInt5("AppUvcd_SendThreadFunc(): still image format idx %d frame idx %d \r\n    dwMaxVideoFrameSize %d\r\n    MaxPayloadTransferSize %d",
                              still_probe_config->bFormatIndex,
                              still_probe_config->bFrameIndex,
                              still_probe_config->dwMaxVideoFrameSize,
                              still_probe_config->dwMaxPayloadTransferSize,
                              0);
            if (size != 0U) {
                if (still_probe_config->dwMaxVideoFrameSize < size) {
                    AppUsb_PrintUInt5("AppUvcd_SendThreadFunc(): dwMaxVideoFrameSize too small, should be larger than real frame size.", 0, 0, 0, 0,0);
                }
                bulk_frame_send(0, 0, size, ptr, UVC_MAX_DATA_SIZE, 1);
                flag_still_capture_ready[stream_id] = 0;
            } else {
                AppUsb_PrintUInt5("AppUvcd_SendThreadFunc(): no still mjpeg data loaded.", 0, 0, 0, 0,0);
            }
        } else {
            // do nothing
        }
    }
    return NULL;
}

static void *thread_push_mjpeg_func(void *Argv)
{
    UVCD_MESSAGE_BODY_s msg = {NULL, 0, 0, 0, 0, 0};
    UINT32 t_cur = 0, t_target = 0, t_send = 0;
    UINT32 *id_ptr;
    UINT32 id;

    AmbaMisra_TouchUnused(Argv); // pass vcast check
    AmbaMisra_TypeCast32(&id_ptr, &Argv);
    AmbaMisra_TouchUnused(id_ptr); // pass vcast check
    id = *id_ptr;

    while (flag_app_uvcd_stop == 0U) {

        if (flag_send_ready[id] == 1U) {
            if (uvc_host_config[id].format_index == 0x01U) {

                UINT32 uRval;
                UINT8 *ptr  = mjpeg_buffer[id];
                UINT32 size = mjpeg_size[id];

                msg.DataBuffer = ptr;
                msg.DataBufferSize = UVC_MAX_DATA_SIZE;
                msg.FrameStartPos = 0; // for MJPEG push thread, always start from 0
                msg.FrameSize = size;
                msg.FrameType = 0;
                msg.StreamID = id;

                // the interval of pushing mjpeg frame is base on frame rate

                t_cur = tx_time_get();
                if (t_target != 0U) {
                    AppUsb_TaskSleep(t_target - t_cur);
                }
                t_send = tx_time_get();

                uRval = AmbaKAL_MsgQueueSend(&message_queue, &msg, AMBA_KAL_NO_WAIT);
                if (uRval != 0U) {
                    AppUsb_PrintUInt5("thread_push_mjpeg_func(): can't send message.", 0, 0, 0, 0, 0);
                }

                t_target = t_send + (1000U / uvc_host_config[id].framerate);

            } else {
                t_target = 0;
                AppUsb_TaskSleep(1000);
            }

        } else {
            t_target = 0;
            AppUsb_TaskSleep(100);
        }
    }
    return NULL;
}

static UINT32 callback_clear_stall(UINT32 LogicalEndpointID)
{
    UINT32 uret;
    if (LogicalEndpointID == 0x81U) {
        uret = encode_stop(0);
    } else if (LogicalEndpointID == 0x83U) {
        uret = encode_stop(1);
    } else {
        AppUsb_PrintUInt5("Don't know how to handle logical endpoint %d.", LogicalEndpointID, 0, 0, 0, 0);
        uret = USB_ERR_FAIL;
    }
    return uret;
}

static void load_still_mjpeg(void)
{
    AMBA_FS_FILE_INFO_s stat;
    AMBA_FS_FILE *f = NULL;
    UINT32 uret;
    UINT32 bytes_read;
    static char fname[] = "c:\\uvc_still_6000x4000.jpg";

    still_mjpeg_size = 0;

    uret = AmbaFS_GetFileInfo(fname, &stat);
    if (uret != 0U) {
        AppUsb_PrintStr5("load_still_mjpeg(): %s doesn't exist.", fname, NULL, NULL, NULL, NULL);
    } else {
        uret = AmbaFS_FileOpen(fname, "r", &f);
        if(uret != 0U) {
            AppUsb_PrintStr5("load_still_mjpeg(): can't open %s", fname, NULL, NULL, NULL, NULL);
        } else {
            uret = AmbaFS_FileRead(still_mjpeg_buffer, 1, (UINT32)stat.Size, f, &bytes_read);
            if (uret != 0U) {
                AppUsb_PrintUInt5("load_still_mjpeg(): File read error", 0, 0, 0, 0, 0);
            } else {
                still_mjpeg_size = (UINT32)stat.Size;
                AppUsb_PrintUInt5("load_still_mjpeg(): still image size %d", still_mjpeg_size, 0, 0, 0, 0);
            }
            uret = AmbaFS_FileClose(f);
            if (uret != 0U) {
                // ignore error
            }
        }
    }
}

static UINT32 resource_create(void)
{
    uvcd_event_callback_t cbs = {NULL};
    UINT32 i, uret = 0;
    UINT32  func_uret;
    static char send_task_name[]  = "AmbaUSBD_RefUVCSendThread";
    static char mjpeg_task_name[] = "AmbaUSBD_RefUVCMjpegThread";
    static char queue_name[] = "AmbaUSBD_RefUVCQueue";
    static UINT8 send_thread_stack[UVCD_SEND_THREAD_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 push_mjpeg_thread_stack[UVCD_MAX_VS_NUMBER][UVCD_MJPEG_THREAD_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static UINT32 id_array[UVCD_MAX_VS_NUMBER] = {0};

    cbs.encode_start = encode_start;
    cbs.encode_stop = encode_stop;
    cbs.still_capture_start = still_capture_start;
    cbs.request_handler = request_handler;

    for (i = 0; i < UVCD_MAX_VS_NUMBER; i++) {
        flag_send_ready[i] = 0;
        uxd_uvc_probe_max_xfer_size_set(i, UVC_MAX_PAYLOAD_SIZE);
    }

    if (thread_send_data.tx_thread_id == 0U) {

        func_uret = AmbaKAL_MsgQueueCreate(&message_queue,
                                           queue_name,
                                           sizeof(UVCD_MESSAGE_BODY_s),
                                           message_queue_buffer,
                                           UVCD_MESSAGE_QUEUE_SIZE);

        if (func_uret != 0U) {
            AppUsb_PrintUInt5("can not create uvc message queue", 0, 0, 0, 0, 0);
            uret = USB_ERR_FAIL;
        } else {
            func_uret = AmbaKAL_TaskCreate(&thread_send_data,
                                           send_task_name,
                                           100,
                                           thread_send_func,
                                           &id_array[0],
                                           send_thread_stack,
                                           UVCD_SEND_THREAD_STACK_SIZE,
                                           AMBA_KAL_AUTO_START);

            if (func_uret != 0U) {
                AppUsb_PrintUInt5("can not create uvc send thread", 0, 0, 0, 0, 0);
                uret = USB_ERR_FAIL;
            }
        }

    } else {
        AppUsb_PrintUInt5("resource_create(): uvc send thread is already created.", 0, 0, 0, 0, 0);
    }

    if (uret == USB_ERR_SUCCESS) {
        for (i = 0; i < UVCD_MAX_VS_NUMBER; i++) {
            if (thread_push_mjpeg[i].tx_thread_id == 0U) {
                id_array[i] = i;
                func_uret = AmbaKAL_TaskCreate(&thread_push_mjpeg[i],
                                               mjpeg_task_name,
                                               100,
                                               thread_push_mjpeg_func,
                                               &id_array[i],
                                               push_mjpeg_thread_stack[i],
                                               UVCD_MJPEG_THREAD_STACK_SIZE,
                                               AMBA_KAL_AUTO_START);
                if (func_uret != 0U) {
                    AppUsb_PrintUInt5("can not create uvc send thread", 0, 0, 0, 0, 0);
                    uret = USB_ERR_FAIL;
                    break;
                }
            }
        }
    }

    load_still_mjpeg();

    if (uxd_uvc_callback_register(&cbs) != USB_ERR_SUCCESS) {
        // should not be here
    }
    if (AppUsbd_ClearStallCBRegister(callback_clear_stall) != USB_ERR_SUCCESS) {
        // should not be here
    }

    AppUsb_PrintUInt5("resource_create(): uvc ut initialized.", 0, 0, 0, 0, 0);

    return uret;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Start UVC device application. It will create related OS resources.
*/
void AppUvcd_Start(void)
{
    static UINT32 init = 0;
    if (init == 0U) {
        UINT32 module_id;
        UINT32 func_uret;
        init_zero();
        flag_app_uvcd_stop = 0;
        (VOID)resource_create();
        init = 1;

        // enable module print for easy debug
        module_id = (USB_ERR_BASE >> 16U);
        func_uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, 1);
        if (func_uret != 0U) {
            AppUsb_PrintUInt5("[AppUvcd_Start] Can't enable module printfor USB, code 0x%X", func_uret, 0, 0, 0, 0);
        }
        module_id = (USBX_ERR_BASE >> 16U);
        func_uret = AmbaPrint_ModuleSetAllowList((UINT16)module_id, 1);
        if (func_uret != 0U) {
            AppUsb_PrintUInt5("[AppUvcd_Start] Can't enable module print for USBX, code 0x%X", func_uret, 0, 0, 0, 0);
        }
    }
}
/**
 * Start UVC device application. It will NOT destroy related OS resources.
*/
void AppUvcd_Stop(void)
{
    // To pass misra-c "function lacks side_effects" issue.
    static UINT32 deinit = 0;
    if (deinit == 0U) {
        // no resource destroy needed
        deinit = 1;
    }
    flag_app_uvcd_stop = 1;
    return;
}
/** @} */
