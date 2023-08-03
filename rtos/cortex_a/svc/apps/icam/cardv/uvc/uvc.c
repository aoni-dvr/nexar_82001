#include "app_base.h"

#if 0
static int img2yuv420(void *img, yuv_frame_info_s *yuv_frame_info)
{
    AMBA_DSP_YUV_IMG_BUF_s *dsp_img = (AMBA_DSP_YUV_IMG_BUF_s *)img;
    int out_w = 0, out_h = 0;
    int j = 0;

    if (dsp_img == NULL) {
        return -1;
    }

    yuv_frame_info->w = out_w = dsp_img->Window.Width;
    yuv_frame_info->h = out_h = dsp_img->Window.Height; 
    if (dsp_img->DataFmt == AMBA_DSP_YUV420) {
        yuv_frame_info->color_fmt = TJSAMP_420;
    } else {
        yuv_frame_info->color_fmt = TJSAMP_422;
    }

    for (j = 0; j < out_h; j++) {
        memcpy(yuv_frame_info->yuv_buffer + (j * out_w), (UINT8 *)(dsp_img->BaseAddrY + (j * dsp_img->Pitch)), out_w);     
    }

    for (j = 0; j < out_h / 2; j++) {
        if (dsp_img->DataFmt == AMBA_DSP_YUV420) {
            int i = 0;
            for (i = 0; i < (out_w / 2); i++) {
                *((UINT8 *)(yuv_frame_info->yuv_buffer + (out_w * out_h) + (j * out_w / 2) + i)) = *((UINT8 *)(dsp_img->BaseAddrUV + j * (dsp_img->Pitch) + (i * 2)));
                *((UINT8 *)(yuv_frame_info->yuv_buffer + (out_w * out_h * 5 >> 2) + (j * out_w / 2) + i)) = *((UINT8 *)(dsp_img->BaseAddrUV + j * (dsp_img->Pitch) + (i * 2) + 1));
            }
        }
    }

    return 0;
}
#endif

static unsigned int zoneId = 0;
static int debug_mode = 0;

int uvc_set_debug(int debug)
{
    debug_mode = debug;

    return 0;
}

static unsigned int cb_on_uvc_ready(const void *pEvent)
{
    const AMBA_DSP_YUV_DATA_RDY_s *pYuvRdy;

    AmbaMisra_TypeCast32(&pYuvRdy, &pEvent);
    if (debug_mode) {
        debug_line("%s,view_zone_id=0x%X,pitch=%d.x=%d,y=%d,w=%d,h=%d. fmt=%d", __func__,
                                                            pYuvRdy->ViewZoneId,
                                                            pYuvRdy->Buffer.Pitch,
                                                            pYuvRdy->Buffer.Window.OffsetX,
                                                            pYuvRdy->Buffer.Window.OffsetY,
                                                            pYuvRdy->Buffer.Window.Width,
                                                            pYuvRdy->Buffer.Window.Height,
                                                            pYuvRdy->Buffer.DataFmt);
    }
    if (pYuvRdy->ViewZoneId == zoneId) {
        int index = 0;
        yuv_frame_info_s *frame = NULL;        
        AMBA_DSP_YUV_IMG_BUF_s *dsp_img = (AMBA_DSP_YUV_IMG_BUF_s *)(&pYuvRdy->Buffer);
        uvc_yuv_queue_get(&frame, &index);
        frame->w = dsp_img->Pitch;
        frame->h = dsp_img->Window.Height;
        memcpy(frame->yuv_buffer, (unsigned char *)dsp_img->BaseAddrY, dsp_img->Pitch * dsp_img->Window.Height);
        uvc_yuv_queue_push(index);
    }

    return 0;
}

int uvc_start(int vin)
{
    if (vin == 0) {
        zoneId = rec_dvr_get_channel_second_stream_zone_id(CAMERA_CHANNEL_INTERNAL);//internal
    } else {
        zoneId = rec_dvr_get_channel_second_stream_zone_id(CAMERA_CHANNEL_EXTERNAL);//external
    }
    uvc_yuv_queue_init();
    uvc_output_task_start();
    AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, cb_on_uvc_ready);

    return 0;
}

int uvc_stop(void)
{
    AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, cb_on_uvc_ready);    
    uvc_output_task_stop();
    uvc_yuv_queue_destroy();

    return 0;
}

